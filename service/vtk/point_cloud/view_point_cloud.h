//
// Created by ByteDance on 12/30/24.
//

#ifndef VTK_VIEW_POINT_CLOUD_H
#define VTK_VIEW_POINT_CLOUD_H

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>
#include <vtkPointSource.h>
#include <vtkCleanPolyData.h>
#include <random>
#include <tuple>
#include <vector>
#include <iostream>

namespace service::vtks::point_cloud {
    struct Point3D {
        double x, y, z;
        Point3D(double x, double y, double z) : x(x), y(y), z(z) {}
    };

    // 生成随机颜色的函数
    std::tuple<float, float, float> generateRandomColor() {
        static std::random_device rd;  // 获取随机数种子
        static std::mt19937 gen(rd());  // 初始化随机数生成器
        std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 均匀分布在 [0.0, 1.0] 之间

        return std::make_tuple(dis(gen), dis(gen), dis(gen));
    }

    /**
     * @brief 可视化点云数据
     * @param point_clouds 点云数据
     */
    void View_point_clouds(const std::vector<std::vector<Point3D>>& point_clouds) {
        std::vector<std::vector<Point3D>> point_clouds_test = {
            {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}}, // 第一个点云
            {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}}  // 第二个点云
        };
        // 创建渲染窗口和交互器
    	vtkNew<vtkNamedColors> colors;
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);
    	renderer->SetBackground(colors->GetColor3d("van_dyke_brown").GetData());
        // 设置渲染窗口的大小
        renderWindow->SetSize(1920, 1080); // 设置宽度为 800，高度为 600
    	vtkNew<vtkRenderWindowInteractor> interactor;
    	interactor->SetRenderWindow(renderWindow.GetPointer());

        vtkNew<vtkRenderWindowInteractor> mouseIterator;
        mouseIterator->SetRenderWindow(renderWindow);

        // 遍历每个点云
        for (size_t i = 0; i < point_clouds_test.size(); ++i) {
            // 创建点云数据
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

            // 添加点到点云
            for (const auto& point : point_clouds[i]) {
                points->InsertNextPoint(point.x, point.y, point.z);
            }

            // 创建 PolyData 对象
            vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
            polyData->SetPoints(points);

            // 创建点云映射器
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(polyData);

            // 创建演员（Actor）
            vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);

            // 生成随机颜色
            auto color = generateRandomColor();
            actor->GetProperty()->SetColor(std::get<0>(color), std::get<1>(color), std::get<2>(color));
            actor->GetProperty()->SetPointSize(5); // 设置点的大小

            // 添加演员到渲染器
            renderer->AddActor(actor);
        }

        renderer->ResetCamera();

        // 开始渲染
        renderWindow->Render();
        interactor->Start();
    }



int  Test_vtk_PointCloud()
{

	// 1 设置点云数据
	vtkNew<vtkNamedColors> colors;

	vtkNew<vtkPointSource> pointSource;
	pointSource->SetNumberOfPoints(50000);
	pointSource->SetRadius(10.0);
	pointSource->Update();
	 // 点云的点数
	auto pts = pointSource->GetNumberOfPoints();

	// 设置第二个窗口的数据
	vtkNew<vtkCleanPolyData> cleanPolyData;
	cleanPolyData->SetInputConnection(pointSource->GetOutputPort());

	cleanPolyData->SetTolerance(0.2);
	cleanPolyData->Update();
	// 第二个窗口 点云的点数
	auto cleanPts = cleanPolyData->GetOutput()->GetNumberOfPoints();

	std::cout << "Original points" << pts << std::endl;
	std::cout << "Cleaned points " << cleanPts << std::endl;
	std::cout << "Reduction      "	<< (1.0 - static_cast<double>(cleanPts) / pts) * 100.0 << "%"<< std::endl;


	// 原始点云  ------------------------------------------------------------
	vtkNew<vtkPolyDataMapper> inputMapper;
	inputMapper->SetInputConnection(pointSource->GetOutputPort());
	 //  原始点云的actor -----
	vtkNew<vtkActor> inputActor;
	inputActor->SetMapper(inputMapper.GetPointer());
	inputActor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());
	inputActor->GetProperty()->SetPointSize(3);
	//==========================================================


	// 第二个窗口的点云  ------------------------------------------------------------
	vtkNew<vtkPolyDataMapper> cleanedMapper;
	cleanedMapper->SetInputConnection(cleanPolyData->GetOutputPort());

	vtkNew<vtkActor> cleanedActor;
	cleanedActor->SetMapper(cleanedMapper.GetPointer());
	cleanedActor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());
	cleanedActor->GetProperty()->SetPointSize(3);
	 //============================================================



	//   定义一个窗口
	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->SetSize(800, 400);
	renderWindow->SetWindowName("Down sample_PointCloud");

	//  一个导演  2个演员
	vtkNew<vtkRenderWindowInteractor> interactor;
	interactor->SetRenderWindow(renderWindow.GetPointer());

	//  定义原始点云和 下采样点云的窗口位置
	double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
	double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

	//  原始电源的渲染
	vtkNew<vtkRenderer> leftRenderer;
	renderWindow->AddRenderer(leftRenderer.GetPointer());
	leftRenderer->SetViewport(leftViewport);
	leftRenderer->SetBackground(colors->GetColor3d("van_dyke_brown").GetData());

	// 下采样点云的渲染
	vtkNew<vtkRenderer> rightRenderer;
	renderWindow->AddRenderer(rightRenderer.GetPointer());
	rightRenderer->SetViewport(rightViewport);
	rightRenderer->SetBackground(colors->GetColor3d("ultramarine").GetData());

	leftRenderer->AddActor(inputActor.GetPointer());   // 原始点云
	rightRenderer->AddActor(cleanedActor.GetPointer());  // 下采样点云



	leftRenderer->ResetCamera();
	rightRenderer->ResetCamera();


	  // 一个窗口  启动
	renderWindow->Render();
	interactor->Start();

	return 0;
}
}

#endif //VTK_VIEW_POINT_CLOUD_H
