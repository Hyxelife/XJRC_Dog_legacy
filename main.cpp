 #include <iostream>
#include <stdio.h>
#include "Console.h"
#include "Controller.h"
#include "AutoControl.h"
#include "Connector.h"
#include "Debug.h"
using namespace std;

#define DEG(deg)    (deg)/180.0f*3.141592653589f
//printf
int main()
{
    Debug::Initialize(NULL,"./log/log.txt",NULL);//"./log/pipe.txt"
    LegStructure::RegisterStructure(LegStructure(9.41f, 25.0f, 25.0f));
    LegMotors::SetMotorScalar(9.1f);
    IMU imuSensor(0,0,0);
    imuSensor.OpenIMU("/dev/ttyUSB4");
    AutoCtrl autoCtrl(&imuSensor);


    Connector connector(Connector::speed,&autoCtrl);
    Controller controller(
        {"/dev/ttyUSB0","/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3"},
        {
 AxisMovement(4.22076,1.34415,4.0735),
 AxisMovement(5.9369,2.872,3.05071),
 AxisMovement(0.0529225,4.32507,5.34133),
 AxisMovement(1.92937,1.81547,0.271899),





















        },

        {
            //AxisMovement(DEG(90),DEG(90),DEG(90)),
            AxisMovement(DEG(23.66),DEG(180-21.45),DEG(18.95)),
            AxisMovement(DEG(23.66),DEG(180-21.45),DEG(19.95)),
            AxisMovement(DEG(21.66),DEG(180-21.45),DEG(18.95)),
            AxisMovement(DEG(23.66),DEG(180-21.45),DEG(18.95)),
        },
        {
            {-1,-1,-1},
            {1,1,1},
            {-1,1,1},
            {1,-1,-1},
        },
        LegController::VMCParam(),
        Controller::CtrlInitParam(0.5,5,14,12,0.3,0.01),
        Controller::MechParam(15+9.41f+9.41f,41)
    );
    Console con("/dev/input/event11",&autoCtrl,&controller);
    OUT("[main]:system ready!\n");




    PacePlanner& planner = controller.GetPacePlanner();
    planner.SetCurveHeight(9.0);
    planner.SetDogHeight(30.0f);

    planner.SetGait(Gait::Pace(0.5f,0.08f),0);
    controller.EnableVMC(false);
    OUT("[main]:params ready!starting up ...\n");

    controller.Start(3.0f);
    OUT("[main]:finish start up procedure!\n");

    OUT("[main]:start loop\n");
    con.Start();
    Console::ConsoleRequest req;
    Console::ConsoleStatus status;
    AutoCtrl::AutoCtrlParam autoPar;
    //while(1);
    bool sysQuit = false;
    bool connectStart = false;
    bool stepOver = false;

    long long debugCnt = 0;
    while(!sysQuit)
    {
        /////////////test area//////////////////////
        //controller.Update(0,0,0,true);
        //continue;
        /////////////end test area//////////////////

        con.GetConsoleRequest(req);
        con.GetConsoleStatus(status);
        if(status.auto_ || status.test)
        {
            //TODO
            if(status.auto_)
            {
                if(!connectStart && false)
                {
                    connector.Start();
                    connectStart = true;
                }
            }
            if(autoCtrl.IsEmpty())
                controller.StopMoving();

            autoCtrl.GetAutoCtrlParam(autoPar);
            //printf("%f,%f,%f\n",autoPar.x,autoPar.y,autoPar.r);
            if(controller.Update(autoPar.x,autoPar.y,autoPar.r,autoPar.hop,autoPar.hopType,true))
                autoCtrl.UpdateStep();
            stepOver = false;
        }else
            stepOver = controller.Update(req.x,req.y,req.r,req.reqHop,req.hopType,true);
        //printf("update\n");
        if(req.reqStop)
        {
            controller.StopMoving();
            //printf("req stop\n");
            }
        con.UpdateEvent(controller.IsStop(),stepOver);
        sysQuit = status.quit;

    }
    OUT("[main]:system quitting...\n");
    controller.Exit();
    OUT("[main]:Controller quit!\n");
    connector.Exit();
    OUT("[Auto]:Connector quit!\n");
    con.Exit();
    OUT("[Console]:Console quit!\n");
    imuSensor.CloseIMU();
    Debug::Exit();
    OUT("[main]:system quit!\n");
    return 0;
}

    Debug::Exit();
    OUT("[main]:system quit!\n");
    return 0;
}
