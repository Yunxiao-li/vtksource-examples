#include "vtkMetaImageReader.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkImageMapper3D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageSlabReslice.h"
#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

using namespace std;
int main() {
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

	static double axialElements[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
    static double sagittalElements[16] = {
        0, 0, 1, 0,
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
    };
    static double coronalElements[16] = {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
    };

	vtkNew<vtkMatrix4x4> resliceAxes;
    resliceAxes->DeepCopy(coronalElements);
    resliceAxes->SetElement(0, 3, center[0] - 0);
    resliceAxes->SetElement(1, 3, center[1]);
	resliceAxes->SetElement(2, 3, center[2]);

	vtkNew<vtkImageSlabReslice> reslice;
	reslice->SetInputConnection(reader->GetOutputPort());
	reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(resliceAxes);

	reslice->SetInterpolationModeToLinear();
    reslice->SetSlabThickness(200);
    //reslice->SetSlabNumberOfSlices(1);
    reslice->SetBlendModeToMax();
    //reslice->SetBlendModeToMean();
    //reslice->SetBlendModeToMin();
	reslice->Update();
    double res = reslice->GetSlabResolution();
    double thick = reslice->GetSlabThickness();
    int numsample = reslice->GetNumBlendSamplePoints();
    int slabnum = reslice->GetSlabNumberOfSlices();
    std::cout << "resolution: " << res << std::endl;
    std::cout << "thickness: " << thick << std::endl;
    std::cout << "num sample: " << numsample << std::endl;
    std::cout << "slabnum: " << slabnum << std::endl;

	vtkNew<vtkLookupTable> colorTable;
    //colorTable->SetRange(-1000, 3700);
    //colorTable->SetTableRange(-1024, 3071);
    colorTable->SetTableRange(-1024, 4071);
	colorTable->SetValueRange(0.0, 1.0);
	colorTable->SetSaturationRange(0.0, 0.0);
    colorTable->SetHueRange(0.0, 0.0);
	colorTable->SetRampToLinear();
	colorTable->Build();

	vtkNew<vtkImageMapToColors> colorMap;
	colorMap->SetLookupTable(colorTable);
	colorMap->SetInputConnection(reslice->GetOutputPort());

	vtkNew<vtkImageActor> imgActor;
	imgActor->GetMapper()->SetInputConnection(colorMap->GetOutputPort());

	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(imgActor);
	renderer->SetBackground(1.0, 1.0, 1.0);

	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->AddRenderer(renderer);
	renderWindow->Render();
	renderWindow->SetSize(640, 480);
	renderWindow->SetWindowName("ImageResliceExample");

	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkNew<vtkInteractorStyleImage> imagestyle;

	renderWindowInteractor->SetInteractorStyle(imagestyle);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
	return 0;
}

