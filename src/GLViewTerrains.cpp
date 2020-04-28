#include "GLViewTerrains.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "WOPxActor.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "PxEngine.h"
#include "WOPxTriangularMesh.h"



//If we want to use way points, we need to include this.
#include "TerrainsWayPoints.h"
#include "io.h";
#include <WOFTGLString.h>
#include <MGLFTGLString.h>
#include <BoxNetMsg.h>


using namespace Aftr;
using namespace physx;

GLViewTerrains* GLViewTerrains::New( const std::vector< std::string >& args )
{
   GLViewTerrains* glv = new GLViewTerrains( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}

void GLViewTerrains::init(float gScalar, Vector gNormVec, std::string confFileName, const PHYSICS_ENGINE_TYPE& physEType) {
    GLView::init(gScalar, gNormVec, confFileName, physEType);
}


GLViewTerrains::GLViewTerrains( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewTerrains::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewTerrains::onCreate() is invoked after this module's LoadMap() is completed.
}

void GLViewTerrains::onKeyDown(const SDL_KeyboardEvent& key)
{
    GLView::onKeyDown(key);
    if (key.keysym.sym == SDLK_0)
        this->setNumPhysicsStepsPerRender(1);

    if (key.keysym.sym == SDLK_1)
    {
        std::getline(std::cin, usermsg);
        std::cout << "Entered Message: " << usermsg << std::endl;
        label->setText(usermsg);

        PlayerMovement msg;
        msg.usermsg = usermsg;
        client->sendNetMsgSynchronousTCP(msg);
    }
    if (key.keysym.sym == SDLK_2)
    {

    }
    if (key.keysym.sym == SDLK_w || key.keysym.sym == SDLK_a || key.keysym.sym == SDLK_s || key.keysym.sym == SDLK_d || key.keysym.sym == SDLK_q || key.keysym.sym == SDLK_e) {
        Vector translation = Vector(0, 0, 0);
        Vector rotation = Vector(0, 0, 0);
        Vector ptrans = Vector(0, 0, 0);
        Vector prot = Vector(0, 0, 0);
        if (key.keysym.sym == SDLK_w)
        {
            translation.x = 1;
            ptrans.x = 1;
        }
        if (key.keysym.sym == SDLK_a)
        {
            translation.y = 1;
            ptrans.y = 1;
        }
        if (key.keysym.sym == SDLK_s)
        {
            translation.x = -1;
            ptrans.x = -1;
        }
        if (key.keysym.sym == SDLK_d)
        {
            translation.y = -1;
            ptrans.y = -1;
        }
        if (key.keysym.sym == SDLK_q)
        {
            rotation.z = 1;
            prot.z = 1;
        }
        if (key.keysym.sym == SDLK_e)
        {
            rotation.z = -1;
            prot.z = -1;
        }
        movePlayer(translation, rotation, ptrans, prot);
        label->setText(usermsg);
    }
    if (key.keysym.sym == SDLK_UP) {
        this->cam->moveInLookDirection(2);
    }
    if (key.keysym.sym == SDLK_DOWN) {
        this->cam->moveOppositeLookDirection(2);
    }
    if (key.keysym.sym == SDLK_LEFT) {
        this->cam->moveLeft();
        this->cam->moveLeft();
        this->cam->moveLeft();
        this->cam->moveLeft();
        this->cam->moveLeft();
    }
    if (key.keysym.sym == SDLK_RIGHT) {
        this->cam->moveRight();
        this->cam->moveRight();
        this->cam->moveRight();
        this->cam->moveRight();
        this->cam->moveRight();
    }
    if (key.keysym.sym == SDLK_x) {
        std::cout << "X Key Pressed" << std::endl;
        spawnItem(Vector(15, 15, 15));
    }
}


void GLViewTerrains::onCreate()
{
    //GLViewTerrains::onCreate() is invoked after this module's LoadMap() is completed.
    //At this point, all the managers are initialized. That is, the engine is fully initialized.

    if (this->pe != NULL)
    {

        this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    this->setActorChaseType(STANDARDEZNAV);
    std::string port = ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort");
    if (port == "12683") {
        client = NetMessengerClient::New("127.0.0.1", "12684");
    }
    else {
        client = NetMessengerClient::New("127.0.0.1", "12683");
    }
}

void GLViewTerrains::updateWorld()
{
    GLView::updateWorld(); //Just call the parent's update world first.
                           //If you want to add additional functionality, do it after
                           //this call.
    this->physics->simulate(this->client, this->boxes);

}


void Aftr::GLViewTerrains::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition(-10,0,15);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );

   /*player = WO::New(ManagerEnvironmentConfiguration::getSMM() + "models/box/box.blend", Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
   player->setPosition(Vector(0, 0, 10));
   worldLst->push_back(player);
   std::string times = ManagerEnvironmentConfiguration::getSMM() + "fonts/TIMES.TTF";
   label = WOFTGLString::New(times, 30);
   usermsg = "Player1";
   label->setText(usermsg);
   label->setPosition(0, 0, 12);
   label->getModelT<MGLFTGLString>()->setSize(2, 2);
   label->getModelT<MGLFTGLString>()->rotateAboutGlobalX(1.55);
   label->getModelT<MGLFTGLString>()->rotateAboutGlobalZ(-1.5);
   worldLst->push_back(label);*/

   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg");

   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 0, 0, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   //wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector( 0, 0, 0 ) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   //grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   //grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   //grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   //grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   //grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   ////Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
   //wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
   //wo->setPosition( Vector(0,0,0) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //Create the infinite grass plane that uses NVIDIAPhysX(the floor)
   /*wo = WONVStaticPlane::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector(0,0,0) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );*/

   ////Create the infinite grass plane (the floor)
   //wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0,50.0f) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //wo->setPosition( Vector(0,0.5f,75.0f) );
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //wo->setLabel( "Grass" );
   //worldLst->push_back( wo );

   //wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue("sharedmultimediapath") + "/models/sphereRp5.wrl", Vector(1.0f, 1.0f, 1.0f), mstSMOOTH );
   //wo->setPosition( 0,0,100.0f );
   //wo->setLabel( "Sphere" );
   //this->worldLst->push_back( wo );

   //wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
   //((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
   //((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
   //wo->setPosition( Vector(20,20,20) );
   //wo->setLabel( "Paladin" );
   //worldLst->push_back( wo );
   //actorLst->push_back( wo );
   //netLst->push_back( wo );
   //this->setActor( wo );
   //
   //wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
   //wo->setPosition( Vector(20,10,20) );
   //wo->isVisible = false; //hide the WOHuman's bounding box
   //((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
   //wo->setLabel( "Human Cyborg" );
   //worldLst->push_back( wo );
   //actorLst->push_back( wo ); //Push the WOHuman as an actor
   //netLst->push_back( wo );
   //this->setActor( wo ); //Start module where human is the actor

   ////Create and insert the WOWheeledVehicle
   //std::vector< std::string > wheels;
   //std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wheels.push_back( wheelStr );
   //wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
   //wo->setPosition( Vector( 5, -15, 20 ) );
   //wo->setLabel( "Car 1970s Beater" );
   //((WOODE*)wo)->mass = 200;
   //worldLst->push_back( wo );
   //actorLst->push_back( wo );
   //this->setActor( wo );
   //netLst->push_back( wo );
   
   createTerrainsWayPoints();
   this->physics = PxEngine::New();
   createGrid();
}


void GLViewTerrains::createTerrainsWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}

void GLViewTerrains::spawnItem(Vector pos) {
    PxTransform trans = PxTransform(PxVec3(pos.x, pos.y, pos.z));
    PxShape* shape = this->physics->phys->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->physics->phys->createMaterial(.5f, .3f, .2f));
    PxRigidDynamic* actor = PxCreateDynamic(*this->physics->phys, trans, *shape, 10.0f);

    WOPxActor* wo = WOPxActor::New(actor, ManagerEnvironmentConfiguration::getLMM() + "/models/box.blend", Vector(1, 1, 1));
    wo->pxActor->userData = wo;
    wo->setPosition(pos);
    worldLst->push_back(wo);
    this->physics->scene->addActor(*wo->pxActor);

    if (this->client != nullptr && this->client->isTCPSocketOpen()) {
        BoxNetMsg msg;
        msg.pos = pos;
        this->client->sendNetMsgSynchronousTCP(msg);
    }

    this->boxes.insert(std::pair(wo, numBoxes));
    numBoxes++;
}

void GLViewTerrains::createGrid() {
    double top = 34.2072593790098f;
    double bottom = 33.9980272592999f;
    double left = -118.65234375f;
    double right = -118.443603515625f;
    double vert = top - bottom;
    double horz = right - left;

    VectorD offset((top + bottom) / 2.0, (left + right) / 2.0, 0.0);

    VectorD scale = VectorD(0.1f, 0.1f, 0.1f);
    VectorD upperLeft(top, left, 0);
    VectorD lowerRight(bottom, right, 0);
    WO* grid = WOPxTriangularMesh::New(upperLeft, lowerRight, 0, offset, scale, ManagerEnvironmentConfiguration::getLMM() + "images/Woodland.tif");

    VectorD ll = ((upperLeft + lowerRight) / 2.0);
    ll.z = 0.0;
    VectorD zdir = ll.toECEFfromWGS84().normalizeMe();
    VectorD northPoleECEF = VectorD(90.0, 0.0, 0.0).toECEFfromWGS84();
    VectorD xdir = northPoleECEF - ll.toECEFfromWGS84();
    xdir = xdir.vectorProjectOnToPlane(zdir);
    xdir.normalize();
    VectorD ydir = zdir.crossProduct(xdir);
    ydir.normalize();
    //create the Local Tangent Plane (LTP) transformation matrix
    float localBodySpaceToLTP[16] = {
        (float)xdir.x, (float)xdir.y, (float)xdir.z, 0.0f,
        (float)ydir.x, (float)ydir.y, (float)ydir.z, 0.0f,
        (float)zdir.x, (float)zdir.y, (float)zdir.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    grid->getModel()->setDisplayMatrix(Mat4(localBodySpaceToLTP).transposeUpperLeft3x3());

    grid->setPosition(0.0f, 0.0f, -50.0f);
    grid->setLabel("grid");
    worldLst->push_back(grid);
    createTriangularMesh(grid);

    // load and set texture to grid
    Texture* tex = ManagerTexture::loadTexture(ManagerEnvironmentConfiguration::getLMM() + "images/Woodland.bmp");
    for (size_t i = 0; i < grid->getModel()->getModelDataShared()->getModelMeshes().size(); i++)
        grid->getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkin().getMultiTextureSet().at(0) = tex;
    grid->getModel()->isUsingBlending(false);
}

void GLViewTerrains::createTriangularMesh(WO* wo) {
    size_t vertexListSize = wo->getModel()->getModelDataShared()->getCompositeVertexList().size();
    size_t indexListSize = wo->getModel()->getModelDataShared()->getCompositeIndexList().size();

    this->vertexListCopy = new float[vertexListSize * 3];
    this->indicesCopy = new unsigned int[indexListSize];

    for (size_t i = 0; i < vertexListSize; i++)
    {
        this->vertexListCopy[i * 3 + 0] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).x;
        this->vertexListCopy[i * 3 + 1] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).y;
        this->vertexListCopy[i * 3 + 2] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).z;
    }
    for (size_t i = 0; i < indexListSize; i++)
        this->indicesCopy[i] = wo->getModel()->getModelDataShared()->getCompositeIndexList().at(i);

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = static_cast<uint32_t>(vertexListSize);
    meshDesc.points.stride = sizeof(float) * 3;
    meshDesc.points.data = vertexListCopy;

    meshDesc.triangles.count = static_cast<uint32_t>(indexListSize) / 3;
    meshDesc.triangles.stride = 3 * sizeof(unsigned int);
    meshDesc.triangles.data = this->indicesCopy;

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;

    bool status = this->physics->cook->cookTriangleMesh(meshDesc, writeBuffer, &result);
    if (!status)
    {
        std::cout << "Failed to create Triangular mesh" << std::endl;
        std::cin.get();
    }
    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxTriangleMesh* mesh = this->physics->phys->createTriangleMesh(readBuffer);

    PxMaterial* gMaterial = this->physics->phys->createMaterial(0.5f, 0.5f, 0.6f);
    PxShape* shape = this->physics->phys->createShape(PxTriangleMeshGeometry(mesh), *gMaterial, true);
    PxTransform t({ 0, 0, 0 });

    Mat4 wo_pose = wo->getModel()->getDisplayMatrix();
    PxRigidStatic* actor = this->physics->phys->createRigidStatic(t);
    bool b = actor->attachShape(*shape);
    float p[] = { wo_pose[0], wo_pose[1], wo_pose[2], wo_pose[3], wo_pose[4], wo_pose[5], wo_pose[6], wo_pose[7], wo_pose[8], wo_pose[9], wo_pose[10], wo_pose[11], wo_pose[12], wo_pose[13], wo_pose[14], wo_pose[15] };
    Vector wo_position = wo->getPosition();
    p[12] = wo_position[0];
    p[13] = wo_position[1];
    p[14] = wo_position[2];
    actor->setGlobalPose(PxTransform(PxMat44(p)));

    this->physics->addActor(wo, actor);
}


void GLViewTerrains::movePlayer(Vector ptrans, Vector prot, Vector ltrans, Vector lrot, bool toSend) {
    player->moveRelative(ptrans);
    player->rotateAboutGlobalX(prot.x);
    player->rotateAboutGlobalY(prot.y);
    player->rotateAboutGlobalZ(prot.z);
    label->moveRelative(ltrans);
    label->rotateAboutGlobalX(lrot.x);
    label->rotateAboutGlobalY(lrot.y);
    label->rotateAboutGlobalZ(lrot.z);

    /*if (toSend) {
        PlayerMovement msg;
        msg.ptranslation = ptrans;
        msg.protation = prot;
        msg.ltranslation = ltrans;
        msg.lrotation = lrot;
        msg.usermsg = usermsg;
        client->sendNetMsgSynchronousTCP(msg);
    }*/
}

void GLViewTerrains::setLabel(std::string newmsg) {
    usermsg = newmsg;
    label->setText(usermsg);
}

GLViewTerrains::~GLViewTerrains()
{

    //Implicitly calls GLView::~GLView()
    NetworkingNetMsg msg;

    delete client;
}


void GLViewTerrains::onResizeWindow(GLsizei width, GLsizei height)
{
    GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewTerrains::onMouseDown(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseDown(e);
}


void GLViewTerrains::onMouseUp(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseUp(e);
}


void GLViewTerrains::onMouseMove(const SDL_MouseMotionEvent& e)
{
    GLView::onMouseMove(e);
}

void GLViewTerrains::onKeyUp(const SDL_KeyboardEvent& key)
{
    GLView::onKeyUp(key);
}

//Skyboxes
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
 //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );