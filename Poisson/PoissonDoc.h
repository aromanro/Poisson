
// PoissonDoc.h : interface of the CPoissonDoc class
//


#pragma once

//#include "vtkPoints.h"
//#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"

#include "PoissonSolver.h"
#include "GaussianChargeDistribution.h"

class CPoissonDoc : public CDocument
{
protected: // create from serialization only
	CPoissonDoc();
	DECLARE_DYNCREATE(CPoissonDoc)

// Attributes
public:
	Fourier::FFT fft;
	Poisson::RealSpaceCell realSpaceCell;
	Poisson::ReciprocalSpaceCell reciprocalCell;

	vtkImageData* densityImage;
	vtkImageData* fieldImage;

// Operations
public:
	void Calculate();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CPoissonDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
