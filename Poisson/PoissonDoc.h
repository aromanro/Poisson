
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


public:
	~CPoissonDoc() override;

// Attributes
	Fourier::FFT fft;
	Poisson::RealSpaceCell realSpaceCell;
	Poisson::ReciprocalSpaceCell reciprocalCell;

	vtkImageData* densityImage;
	vtkImageData* fieldImage;

// Operations
	void Calculate();

// Overrides
private:
	BOOL OnNewDocument() override;
	void Serialize(CArchive& ar) override;
#ifdef SHARED_HANDLERS
	void InitializeSearchContent() override;
	void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) override;
#endif // SHARED_HANDLERS

// Implementation

#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
