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
#include <vtkAxesActor.h>
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

	// 函数将 std::vector<std::vector<Point3D>> 转换为 vtkPoints
	vtkNew<vtkPoints> convertToVTKPoints(const std::vector<Point3D>& points) {
    	vtkNew<vtkPoints> vtk_points;
    	for (const auto& point : points) {
    		auto res = vtk_points->InsertNextPoint(point.x, point.y, point.z);
    		// std::cout << res << std::endl;
    	}
    	return vtk_points;
    }

    /**
     * @brief 可视化点云数据
     * @param point_clouds 点云数据
     */
    void View_point_clouds(const std::vector<std::vector<Point3D>>& point_clouds) {
    	vtkNew<vtkNamedColors> colors;
        // 创建一个渲染器
        vtkNew<vtkRenderer> renderer;

        // 随机颜色生成函数
        auto generateRandomColor = [&]() -> std::tuple<float, float, float> {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(0.0f, 1.0f);
            return std::make_tuple(dis(gen), dis(gen), dis(gen));
        };

        // 创建多个点云并赋予颜色
        for (size_t i = 0; i < point_clouds.size(); ++i) {
            // 获取点云数据
            vtkNew<vtkPoints> points = convertToVTKPoints(point_clouds[i]);

            // 创建一个点云 PolyData
            vtkNew<vtkPolyData> polyData;
            polyData->SetPoints(points.GetPointer());
        	std::cout << polyData->GetNumberOfPoints() << std::endl;

            // 创建一个点云的Mapper
            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputData(polyData.GetPointer());

            // 创建一个演员（Actor）以便将点云添加到渲染器
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper.GetPointer());

            // 随机颜色
            auto color = generateRandomColor();
            actor->GetProperty()->SetColor(std::get<0>(color), std::get<1>(color), std::get<2>(color));
        	actor->GetProperty()->SetPointSize(5); // 设置点大小

            // 将 actor 添加到渲染器
            renderer->AddActor(actor.GetPointer());
        }

        // 设置背景色
        // vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
        // renderer->SetBackground(colors->GetColor3d("DarkSlateGray"));

        // 创建渲染窗口
        vtkNew<vtkRenderWindow> renderWindow;
    	renderWindow->SetSize(1920, 1080);
    	renderWindow->SetWindowName("PointCloud");
        // 将渲染器添加到渲染窗口
        renderWindow->AddRenderer(renderer.GetPointer());

        // 创建交互器
        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

        // 设置坐标轴
        vtkNew<vtkAxesActor> axes;
        renderer->AddActor(axes.GetPointer());

    	vtkNew<vtkPointSource> pointSource;
    	pointSource->SetNumberOfPoints(50000);
    	pointSource->SetRadius(10.0);
    	pointSource->Update();

    	vtkNew<vtkCleanPolyData> cleanPolyData;
    	cleanPolyData->SetInputConnection(pointSource->GetOutputPort());

    	cleanPolyData->SetTolerance(0.2);
    	cleanPolyData->Update();

    	vtkNew<vtkPolyDataMapper> cleanedMapper;
    	cleanedMapper->SetInputConnection(cleanPolyData->GetOutputPort());

    	vtkNew<vtkActor> cleanedActor;
    	cleanedActor->SetMapper(cleanedMapper.GetPointer());
    	cleanedActor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());
    	cleanedActor->GetProperty()->SetPointSize(3);

    	renderer->AddActor(cleanedActor.GetPointer());

        // 启动渲染并进行交互
        renderWindow->Render();
        renderWindowInteractor->Start();
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
