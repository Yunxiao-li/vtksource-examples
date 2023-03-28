/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Medical3.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//
// This example reads a volume dataset, extracts two isosurfaces that
// represent the skin and bone, creates three orthogonal planes
// (sagittal, axial, coronal), and displays them.
//
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkLookupTable.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStripper.h>
#include <vtkVolume16Reader.h>
#include "vtkDICOMImageReader.h"
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

int main(int argc, char* argv[])
{
  // if (argc < 2)
  // {
  //   cout << "Usage: " << argv[0] << " DATADIR/headsq/quarter" << endl;
  //   return EXIT_FAILURE;
  // }

  // Create the renderer, the render window, and the interactor. The
  // renderer draws into the render window, the interactor enables
  // mouse- and keyboard-based interaction with the data within the
  // render window.
  //
  vtkSmartPointer<vtkRenderer> aRenderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer(aRenderer);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);

  // Set a background color for the renderer and set the size of the
  // render window (expressed in pixels).
  aRenderer->SetBackground(.2, .3, .4);
  renWin->SetSize(640, 480);

  vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName("../data/image_04");
	reader->Update();

	int extent[6];
	double spacing[3];
	double origin[3];

	reader->GetOutput()->GetExtent(extent);
	reader->GetOutput()->GetSpacing(spacing);
	reader->GetOutput()->GetOrigin(origin);
    vtkImageData* image = reader->GetOutput();
    double* scalarrange = image->GetScalarRange();
    std::cout << "scalar range: " << scalarrange[0] << "  " << scalarrange[1] << std::endl;
    std::cout << "extent: " << extent[0] << " " << extent[1] << " " << extent[2]
              << " " << extent[3] << " " << extent[4] << " " << extent[5] << std::endl;
    std::cout << "spacing: " << spacing[0] << " " << spacing[1] << " " << spacing[2] << std::endl;
    std::cout << "origin: " << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;
    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
    reader->GetOutput()->SetOrigin(center);

  // Now we are creating three orthogonal planes passing through the
  // volume. Each plane uses a different texture map and therefore has
  // different coloration.

  // Start by creating a black/white lookup table.
  vtkSmartPointer<vtkLookupTable> bwLut = vtkSmartPointer<vtkLookupTable>::New();
  bwLut->SetTableRange(0, 5000);
  bwLut->SetSaturationRange(0, 0);
  bwLut->SetHueRange(0, 0);
  bwLut->SetValueRange(0, 1);
  bwLut->Build(); // effective built

  // // Now create a lookup table that consists of the full hue circle
  // // (from HSV).
  // vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
  // hueLut->SetTableRange(0, 2000);
  // hueLut->SetHueRange(0, 1);
  // hueLut->SetSaturationRange(1, 1);
  // hueLut->SetValueRange(1, 1);
  // hueLut->Build(); // effective built

  // // Finally, create a lookup table with a single hue but having a range
  // // in the saturation of the hue.
  // vtkSmartPointer<vtkLookupTable> satLut = vtkSmartPointer<vtkLookupTable>::New();
  // satLut->SetTableRange(0, 2000);
  // satLut->SetHueRange(.6, .6);
  // satLut->SetSaturationRange(0, 1);
  // satLut->SetValueRange(1, 1);
  // satLut->Build(); // effective built

  // Create the first of the three planes. The filter vtkImageMapToColors
  // maps the data through the corresponding lookup table created above.  The
  // vtkImageActor is a type of vtkProp and conveniently displays an image on
  // a single quadrilateral plane. It does this using texture mapping and as
  // a result is quite fast. (Note: the input image has to be unsigned char
  // values, which the vtkImageMapToColors produces.) Note also that by
  // specifying the DisplayExtent, the pipeline requests data of this extent
  // and the vtkImageMapToColors only processes a slice of data.
  vtkSmartPointer<vtkImageMapToColors> sagittalColors = vtkSmartPointer<vtkImageMapToColors>::New();
  //sagittalColors->SetInputData(image);
  sagittalColors->SetInputConnection(reader->GetOutputPort());
  sagittalColors->SetLookupTable(bwLut);
  sagittalColors->Update();

  vtkSmartPointer<vtkImageActor> sagittal = vtkSmartPointer<vtkImageActor>::New();
  //sagittal->SetInputData(sagittalColors->GetOutput());
  sagittal->GetMapper()->SetInputConnection(sagittalColors->GetOutputPort());
  sagittal->SetDisplayExtent(0, 511, 0, 511, 20, 20);
  //sagittal->SetDisplayExtent(32, 32, 0, 63, 0, 92);
//   sagittal->ForceOpaqueOn();

  // // Create the second (axial) plane of the three planes. We use the
  // // same approach as before except that the extent differs.
  // vtkSmartPointer<vtkImageMapToColors> axialColors = vtkSmartPointer<vtkImageMapToColors>::New();
  // axialColors->SetInputConnection(image->GetOutputPort());
  // axialColors->SetLookupTable(hueLut);
  // axialColors->Update();

  // vtkSmartPointer<vtkImageActor> axial = vtkSmartPointer<vtkImageActor>::New();
  // axial->GetMapper()->SetInputConnection(axialColors->GetOutputPort());
  // axial->SetDisplayExtent(0, 63, 0, 63, 46, 46);
  // axial->ForceOpaqueOn();

  // // Create the third (coronal) plane of the three planes. We use
  // // the same approach as before except that the extent differs.
  // vtkSmartPointer<vtkImageMapToColors> coronalColors = vtkSmartPointer<vtkImageMapToColors>::New();
  // coronalColors->SetInputConnection(image->GetOutputPort());
  // coronalColors->SetLookupTable(satLut);
  // coronalColors->Update();

  // vtkSmartPointer<vtkImageActor> coronal = vtkSmartPointer<vtkImageActor>::New();
  // coronal->GetMapper()->SetInputConnection(coronalColors->GetOutputPort());
  // coronal->SetDisplayExtent(0, 63, 32, 32, 0, 92);
  // coronal->ForceOpaqueOn();

  // It is convenient to create an initial view of the data. The
  // FocalPoint and Position form a vector direction. Later on
  // (ResetCamera() method) this vector is used to position the camera
  // to look at the data in this direction.
  // vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();
  // aCamera->SetViewUp(0, 0, -1);
  // aCamera->SetPosition(0, 1, 0);
  // aCamera->SetFocalPoint(0, 0, 0);
  // aCamera->ComputeViewPlaneNormal();
  // aCamera->Azimuth(30.0);
  // aCamera->Elevation(30.0);

  // Actors are added to the renderer.
  // aRenderer->AddActor(outline);
  aRenderer->AddActor(sagittal);
  // aRenderer->AddActor(axial);
  // aRenderer->AddActor(coronal);
  // aRenderer->AddActor(skin);
  // aRenderer->AddActor(bone);

  // Turn off bone for this example.
  // bone->VisibilityOff();

  // Set skin to semi-transparent.
  // skin->GetProperty()->SetOpacity(0.5);

  // An initial camera view is created.  The Dolly() method moves
  // the camera towards the FocalPoint, thereby enlarging the image.
  // aRenderer->SetActiveCamera(aCamera);

  // Calling Render() directly on a vtkRenderer is strictly forbidden.
  // Only calling Render() on the vtkRenderWindow is a valid call.
  renWin->Render();

  aRenderer->ResetCamera();
  // aCamera->Dolly(1.5);

  // Note that when camera movement occurs (as it does in the Dolly()
  // method), the clipping planes often need adjusting. Clipping planes
  // consist of two planes: near and far along the view direction. The
  // near plane clips out objects in front of the plane; the far plane
  // clips out objects behind the plane. This way only what is drawn
  // between the planes is actually rendered.
  // aRenderer->ResetCameraClippingRange();
  // interact with data
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
