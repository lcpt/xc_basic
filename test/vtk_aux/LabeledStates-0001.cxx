// Prueba de vtkLabeledShStrMapper.h basada en el código de
// Eric F. Sorton

//LCPT.

#include "vtkCellCenters.h"
#include "vtkDataArray.h"
#include "vtkAbstractArray.h"
#include "xc_basic/src/vtk_aux/vtkShStrArray.h"
#include "vtkFeatureEdges.h"
#include "xc_basic/src/vtk_aux/vtkLabeledShStrMapper.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"

char fixedArray[51][3] = {
	"AK", "AL", "AR", "AZ", "CA", "CO", "CT", "DC", "DE", "FL", "GA",
	"HI", "IA", "ID", "IL", "IN", "KS", "KY", "LA", "MA", "MD",
	"ME", "MI", "MN", "MO", "MS", "MT", "NC", "ND", "NE", "NH",
	"NJ", "NM", "NV", "NY", "OH", "OK", "OR", "PA", "RI", "SC",
	"SD", "TN", "TX", "UT", "VA", "VT", "WA", "WI", "WV", "WY"
};

int main( int argc, char *argv[] )
{
  vtkShStrArray *stateLabels = vtkShStrArray::New();
    stateLabels->SetNumberOfValues(51);
    for(size_t i=0;i<51;i++)
      stateLabels->InsertValue(i,std::string(fixedArray[i]));

  vtkPolyDataReader *states = vtkPolyDataReader::New();
    states->SetFileName("usa.vtk");
    states->Update();
    states->GetOutput()->GetCellData()->AddArray(stateLabels);

  vtkFeatureEdges *featureEdges = vtkFeatureEdges::New();
    featureEdges->BoundaryEdgesOn();
    featureEdges->ManifoldEdgesOn();
    featureEdges->NonManifoldEdgesOn();
    featureEdges->FeatureEdgesOn();
    featureEdges->SetInput(states->GetOutput());
  
  vtkPolyDataMapper *statesMapper = vtkPolyDataMapper::New();
    statesMapper->SetInput(featureEdges->GetOutput());

  vtkActor *statesActor = vtkActor::New();
    statesActor->SetMapper(statesMapper);

  vtkCellCenters *cellCenters = vtkCellCenters::New();
    cellCenters->SetInput(states->GetOutput());

  vtkLabeledShStrMapper *labelMapper = vtkLabeledShStrMapper::New();
    labelMapper->SetInput(cellCenters->GetOutput());

  vtkActor2D *labelActor = vtkActor2D::New();
    labelActor->SetMapper(labelMapper);

  vtkRenderer *ren1= vtkRenderer::New();
    ren1->AddActor(statesActor);
    ren1->AddActor(labelActor);
    ren1->SetBackground( 0.1, 0.2, 0.4 );

  vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer( ren1 );
    renWin->SetSize( 300, 300 );

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);
    iren->Start();

  stateLabels->Delete();
  states->Delete();
  featureEdges->Delete();
  statesMapper->Delete();
  statesActor->Delete();
  cellCenters->Delete();
  labelMapper->Delete();
  labelActor->Delete();
  ren1->Delete();
  renWin->Delete();
  //  iren->Delete();

  return 0;
}


