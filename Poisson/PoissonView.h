
// PoissonView.h : interface of the CPoissonView class
//

#pragma once

#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkWin32RenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "vtkImageData.h"
#include "vtkImageDataGeometryFilter.h"

#include "vtkDataSetMapper.h"
#include "vtkWarpScalar.h"

#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkInformation.h"

#include "vtkAxisActor2D.h"
#include "vtkCubeAxesActor2D.h"

#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"



class CPoissonView : public CView
{
protected: // create from serialization only
	CPoissonView();
	DECLARE_DYNCREATE(CPoissonView)

// Attributes
public:
	CPoissonDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CPoissonView();

private:
	void Pipeline();

	void PipelineView(vtkRenderer *ren, vtkImageDataGeometryFilter* geometryFilter, vtkWarpScalar* warp, vtkDataSetMapper* mapper, vtkActor* chartActor, vtkCubeAxesActor2D* axes);


	vtkWin32OpenGLRenderWindow *renWin;
	vtkRenderer *ren1;
	vtkRenderer *ren2;
	vtkWin32RenderWindowInteractor *iren;

	vtkImageDataGeometryFilter* geometryFilter1;
	vtkImageDataGeometryFilter* geometryFilter2;


	vtkWarpScalar* warp1;
	vtkDataSetMapper* mapper1;
	vtkActor* chartActor1;

	vtkWarpScalar* warp2;
	vtkDataSetMapper* mapper2;
	vtkActor* chartActor2;


	vtkCubeAxesActor2D* axes1;
	vtkCubeAxesActor2D* axes2;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // debug version in PoissonView.cpp
inline CPoissonDoc* CPoissonView::GetDocument() const
   { return reinterpret_cast<CPoissonDoc*>(m_pDocument); }
#endif

