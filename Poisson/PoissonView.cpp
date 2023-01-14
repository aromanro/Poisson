
// PoissonView.cpp : implementation of the CPoissonView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Poisson.h"
#endif

#include "PoissonDoc.h"
#include "PoissonView.h"

#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CPoissonView

IMPLEMENT_DYNCREATE(CPoissonView, CView)

BEGIN_MESSAGE_MAP(CPoissonView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPoissonView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CPoissonView construction/destruction

CPoissonView::CPoissonView()
	: ren1(nullptr), ren2(nullptr), iren(nullptr), renWin(nullptr)
{
	// Create the objects used to form the visualization.

	warp1 = vtkWarpScalar::New();
	mapper1 = vtkDataSetMapper::New();
	chartActor1 = vtkActor::New();

	warp2 = vtkWarpScalar::New();
	mapper2 = vtkDataSetMapper::New();
	chartActor2 = vtkActor::New();

	geometryFilter1 =  vtkImageDataGeometryFilter::New();
	geometryFilter2 =  vtkImageDataGeometryFilter::New();


	axes1 = vtkCubeAxesActor2D::New();
	axes2 = vtkCubeAxesActor2D::New();
}

CPoissonView::~CPoissonView()
{
	// Delete the renderer, window and interactor objects.

	if (ren1)
	{
		ren1->Delete();
		ren2->Delete();
		iren->Delete();
		renWin->Delete();
	}


	// Delete the objects used to form the visualization.
	warp1->Delete();
	mapper1->Delete();
	chartActor1->Delete();

	warp2->Delete();
	mapper2->Delete();
	chartActor2->Delete();

	geometryFilter1->Delete();
	geometryFilter2->Delete();
	axes1->Delete();
	axes2->Delete();
}



// CPoissonView drawing

void CPoissonView::OnDraw(CDC* pDC)
{
	const CPoissonDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (!iren || !renWin)return;

	if (!iren->GetInitialized())
	{
		iren->SetRenderWindow(renWin);

		CRect rect;

		GetClientRect(&rect);
		iren->Initialize();
		renWin->SetSize(rect.right - rect.left, rect.bottom - rect.top);
		ren1->ResetCamera();
		ren2->ResetCamera();
	}

			
	if (pDC->IsPrinting())
	{
		BeginWaitCursor();

		// Obtain the size of the printer page in pixels.

		const int cxPage = pDC->GetDeviceCaps(HORZRES);
		const int cyPage = pDC->GetDeviceCaps(VERTRES);

		// Get the size of the window in pixels.

		const int *size = renWin->GetSize();
		int cxWindow = size[0];
		int cyWindow = size[1];
		const float fx = static_cast<float>(cxPage) / static_cast<float>(cxWindow);
		const float fy = static_cast<float>(cyPage) / static_cast<float>(cyWindow);
		const float scale = min(fx, fy);
		const int x = static_cast<int>(scale * static_cast<float>(cxWindow));
		const int y = static_cast<int>(scale * static_cast<float>(cyWindow));

		// this is from VTK-8.0.1\GUISupport\MFC\vtkMFCWindow.cpp
		// with some corrections, they don't do DeleteObject and DeleteDC and here for some reason delete[] pixels crashes

		renWin->SetUseOffScreenBuffers(true);
		renWin->Render();
		
		const unsigned char *pixels = renWin->GetPixelData(0,0,cxWindow-1,cyWindow-1,0, NULL);

		int dataWidth = ((cxWindow*3+3)/4)*4;

		BITMAPINFO MemoryDataHeader;
		MemoryDataHeader.bmiHeader.biSize = 40;
		MemoryDataHeader.bmiHeader.biWidth = cxWindow;
		MemoryDataHeader.bmiHeader.biHeight = cyWindow;
		MemoryDataHeader.bmiHeader.biPlanes = 1;
		MemoryDataHeader.bmiHeader.biBitCount = 24;
		MemoryDataHeader.bmiHeader.biCompression = BI_RGB;
		MemoryDataHeader.bmiHeader.biClrUsed = 0;
		MemoryDataHeader.bmiHeader.biClrImportant = 0;
		MemoryDataHeader.bmiHeader.biSizeImage = dataWidth*cyWindow;
		MemoryDataHeader.bmiHeader.biXPelsPerMeter = 10000;
		MemoryDataHeader.bmiHeader.biYPelsPerMeter = 10000;

		unsigned char *MemoryData = NULL;
		HDC MemoryHdc = static_cast<HDC>(CreateCompatibleDC(pDC->GetSafeHdc()));
		HBITMAP dib = CreateDIBSection(MemoryHdc, &MemoryDataHeader, DIB_RGB_COLORS, (void **)(&MemoryData),  NULL, 0);

		if (dib)
		{
			// copy the pixels over
			for (int i = 0; i < cyWindow; ++i)
				for (int j = 0; j < cxWindow; ++j)
				{
					MemoryData[i*dataWidth + j*3] = pixels[i*cxWindow*3 + j*3 + 2];
					MemoryData[i*dataWidth + j*3 + 1] = pixels[i*cxWindow*3 + j*3 + 1];
					MemoryData[i*dataWidth + j*3 + 2] = pixels[i*cxWindow*3 + j*3];
				}


			SelectObject(MemoryHdc, dib);
			StretchBlt(pDC->GetSafeHdc(), 0, 0, x, y, MemoryHdc, 0, 0, cxWindow, cyWindow, SRCCOPY);

			renWin->SetUseOffScreenBuffers(false);

			DeleteObject(dib);
		}

		DeleteDC(MemoryHdc);

		// in debug this crashes
#ifndef _DEBUG
		delete[] pixels;
#endif

		EndWaitCursor();
	}
	else
	{
		renWin->Render();
	}
}


// CPoissonView printing


void CPoissonView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPoissonView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPoissonView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPoissonView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CPoissonView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPoissonView::OnContextMenu(CWnd* /* pWnd */, CPoint /*point*/)
{
#ifndef SHARED_HANDLERS
#endif
}


// CPoissonView diagnostics

#ifdef _DEBUG
void CPoissonView::AssertValid() const
{
	CView::AssertValid();
}

void CPoissonView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPoissonDoc* CPoissonView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPoissonDoc)));
	return dynamic_cast<CPoissonDoc*>(m_pDocument);
}
#endif //_DEBUG


// CPoissonView message handlers


void CPoissonView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rect;

	GetClientRect(&rect);

	if (renWin) 
	{
		renWin->SetSize(rect.right - rect.left, rect.bottom - rect.top);
		if (iren) iren->UpdateSize(rect.Width(), rect.Height());
	}
}


BOOL CPoissonView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}


LRESULT CPoissonView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_CHAR:
	case WM_TIMER:
		if (iren && iren->GetInitialized() && GetSafeHwnd())
		{
			return vtkHandleMessage(GetSafeHwnd(), message, wParam, lParam);
			//return vtkHandleMessage2(GetSafeHwnd(), message, wParam, lParam, iren);
		}
	break;
  }

	return CView::WindowProc(message, wParam, lParam);
}

void CPoissonView::Pipeline()
{
	PipelineView(ren1, geometryFilter1, warp1, mapper1, chartActor1, axes1);
	PipelineView(ren2, geometryFilter2, warp2, mapper2, chartActor2, axes2);
}


void CPoissonView::PipelineView(vtkRenderer *ren, vtkImageDataGeometryFilter* geometryFilter, vtkWarpScalar* warp, vtkDataSetMapper* mapper, vtkActor* chartActor, vtkCubeAxesActor2D* axes)
{
	warp->SetInputConnection(geometryFilter->GetOutputPort());	
  
	//mapper->SetInputConnection(warp->GetOutputPort());
	
	//***************************************************************************************
	// Gouraud shading needs normals. Just uncomment the above and comment what follows up to //****
	// to remove shading
	
	vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
	normals->SetInputConnection(warp->GetOutputPort());
	normals->SplittingOff();
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOff();
	normals->ConsistencyOn();
	mapper->SetInputConnection(normals->GetOutputPort());
	
	//****************************************************************************************

	chartActor->SetMapper(mapper);
	chartActor->GetProperty()->SetInterpolationToGouraud();
					
	ren->AddActor(chartActor);
	
	// add & render CubeAxes
	
	axes->SetInputData(warp->GetOutput());
	axes->SetCamera(ren->GetActiveCamera());  
	ren->AddViewProp(axes);
}




void CPoissonView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CPoissonDoc* pDoc = GetDocument();
	
	geometryFilter1->SetInputData(pDoc->densityImage);
	geometryFilter2->SetInputData(pDoc->fieldImage);

	warp1->SetScaleFactor(1.0);
	warp2->SetScaleFactor(1.0);


	// Create the renderer, window and interactor objects.

	ren1 = vtkRenderer::New();
	ren2 = vtkRenderer::New();
	
	renWin = vtkWin32OpenGLRenderWindow::New();
	iren = vtkWin32RenderWindowInteractor::New();


	renWin->AddRenderer(ren1);
	renWin->AddRenderer(ren2);

	// setup the parent window

	renWin->SetParentId(GetSafeHwnd());		

	iren->SetRenderWindow(renWin);

	CRect rect;

	GetClientRect(&rect);
	iren->Initialize();
	renWin->SetSize(rect.right - rect.left, rect.bottom - rect.top);


	ren1->GetActiveCamera()->SetFocalPoint(pDoc->realSpaceCell.GetSize().Y/2, pDoc->realSpaceCell.GetSize().Z/2, 0);
	ren1->GetActiveCamera()->SetPosition(0, -pDoc->realSpaceCell.GetSize().Z * 2, pDoc->realSpaceCell.GetSize().Z * 2);
	ren1->GetActiveCamera()->Elevation(10);	
	ren1->GetActiveCamera()->ComputeViewPlaneNormal();

	ren1->SetBackground(0.1, 0.3, 0.4);	
	ren1->SetViewport(0., 0., 0.5, 1.);

	ren2->GetActiveCamera()->SetFocalPoint(pDoc->realSpaceCell.GetSize().Y/2, pDoc->realSpaceCell.GetSize().Z/2, 0);
	ren2->GetActiveCamera()->SetPosition(0, -pDoc->realSpaceCell.GetSize().Z * 2, pDoc->realSpaceCell.GetSize().Z * 2);
	ren2->GetActiveCamera()->Elevation(10);
	ren2->GetActiveCamera()->ComputeViewPlaneNormal();

	ren2->SetBackground(0.1, 0.25, 0.4);	
	ren2->SetViewport(0.5, 0., 1., 1.);

	axes1->SetFontFactor(0.5);
	//axes1->SetFlyModeToNone();
	axes1->UseBoundsOn();

	vtkSmartPointer<vtkAxisActor2D> zAxis = axes1->GetZAxisActor2D();
	zAxis->SetAdjustLabels(1);	


	axes2->SetFontFactor(0.5);
	//axes2->SetFlyModeToNone();
	axes2->UseBoundsOn();

	zAxis = axes2->GetZAxisActor2D();
	zAxis->SetAdjustLabels(1);	

	// Gouraud shading needs those

	vtkSmartPointer<vtkProperty> prop = vtkSmartPointer<vtkProperty>::New();
	prop->SetDiffuseColor(0.5, 0.5, 0.5);
	prop->SetAmbientColor(0.5, 0.5, 0.5);
	prop->SetSpecularPower(30);
	prop->SetSpecular(.4);
	prop->SetDiffuse(.6);
	prop->SetAmbient(.1);
	chartActor1->SetProperty(prop.GetPointer());
	chartActor2->SetProperty(prop.GetPointer());

	// end of settings for shading

	vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetInput("Charge Density");
	textActor->SetPosition(10, 10);
	textActor->GetTextProperty()->SetFontSize(36);
	textActor->GetTextProperty()->SetColor(0., 1.0, 0.);
	ren1->AddActor2D(textActor);

	textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetInput("Field");
	textActor->SetPosition(10, 10);
	textActor->GetTextProperty()->SetFontSize(36);
	textActor->GetTextProperty()->SetColor(0., 1.0, 0.);
	ren2->AddActor2D(textActor);

	Pipeline();
}
