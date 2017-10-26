
// PoissonDoc.cpp : implementation of the CPoissonDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Poisson.h"
#endif

#include "PoissonDoc.h"



#include <iostream>
#include <fstream>



#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPoissonDoc

IMPLEMENT_DYNCREATE(CPoissonDoc, CDocument)

BEGIN_MESSAGE_MAP(CPoissonDoc, CDocument)
END_MESSAGE_MAP()


// CPoissonDoc construction/destruction

CPoissonDoc::CPoissonDoc()
	:  realSpaceCell(6, 6, 6, 64, 64, 64),
	reciprocalCell(realSpaceCell)
{
	densityImage = vtkImageData::New();
	fieldImage = vtkImageData::New();

	densityImage->SetSpacing(realSpaceCell.GetSize().Y/realSpaceCell.GetSamples().Y, realSpaceCell.GetSize().Z/realSpaceCell.GetSamples().Z, 0);
	densityImage->SetDimensions(realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z, 1); //number of points in each direction

	fieldImage->SetSpacing(realSpaceCell.GetSize().Y/realSpaceCell.GetSamples().Y, realSpaceCell.GetSize().Z/realSpaceCell.GetSamples().Z, 0);
	fieldImage->SetDimensions(realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z, 1); //number of points in each direction

	densityImage->AllocateScalars(VTK_FLOAT, 1);
	fieldImage->AllocateScalars(VTK_FLOAT, 1);

	Calculate();
}

CPoissonDoc::~CPoissonDoc()
{
	densityImage->Delete();
	fieldImage->Delete();
}

BOOL CPoissonDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	SetTitle(L"Charge distribution and potential");

	return TRUE;
}


void CPoissonDoc::Calculate()
{
	double sigma = 0.25;


	TRACE(L"Cell volume: %f, nr. samples: %d\n", realSpaceCell.Volume(), realSpaceCell.Samples());


	// add charge distributions to the real space cell

	Poisson::GaussianChargeDistribution charge;

	Poisson::Charges charges;
	charge.position = Vector3D<double>(0, 0, 0);
	charges.charges.push_back(charge);

	charge.position = Vector3D<double>(0, 0, 4);
	charges.charges.push_back(charge);

	charges.ComputeStructureFactorAndChargeDensity(fft, realSpaceCell, reciprocalCell, sigma);
	
	TRACE(L"Total charge check: %f\n", charges.ChargeDensity.sum().real()*realSpaceCell.SampleVolume());

	// done with charges

	// now slice it - put the values in the 'image' data for VTK

	const unsigned int start = realSpaceCell.GetSamples().X * realSpaceCell.GetSamples().Y * realSpaceCell.GetSamples().Z / 2;

	for (unsigned int i = 0; i < realSpaceCell.GetSamples().Y; ++i)
		for (unsigned int j = 0; j < realSpaceCell.GetSamples().Z; ++j)
		{
			unsigned int pos = start + realSpaceCell.GetSamples().Z * i + j;

			densityImage->SetScalarComponentFromDouble(i, j, 0, 0, charges.ChargeDensity(pos).real());
		}
	


	// solve it

	Eigen::VectorXcd field = Poisson::PoissonSolver::SolveToRealSpace(fft, realSpaceCell, reciprocalCell, charges);

	// slice the result - put the values in the 'image' data for VTK

	for (unsigned int i = 0; i < realSpaceCell.GetSamples().Y; ++i)
		for (unsigned int j = 0; j < realSpaceCell.GetSamples().Z; ++j)
		{
			unsigned int pos = start + realSpaceCell.GetSamples().Z * i + j;

			fieldImage->SetScalarComponentFromDouble(i, j, 0, 0, field(pos).real());
		}
	

	// compute approximate Ewald energy

	Eigen::VectorXcd fieldReciprocal(realSpaceCell.Samples());
	Eigen::VectorXcd densityReciprocal(realSpaceCell.Samples());

	fft.fwd(field.data(), fieldReciprocal.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);
	fft.fwd(charges.ChargeDensity.data(), densityReciprocal.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);
	
	
	densityReciprocal *= realSpaceCell.SampleVolume();
	fieldReciprocal  /= realSpaceCell.Samples();
		
	double numericalResult = (fieldReciprocal.adjoint() * densityReciprocal)(0).real()/2.;

	double Uself=1./(2*sqrt(M_PI))*(1/sigma)*charges.charges.size();

	TRACE(L"Ewald energy: %f\n", numericalResult - Uself);
}





// CPoissonDoc serialization

void CPoissonDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CPoissonDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CPoissonDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CPoissonDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CPoissonDoc diagnostics

#ifdef _DEBUG
void CPoissonDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPoissonDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPoissonDoc commands
