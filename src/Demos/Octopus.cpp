#include "stdafx.h"

#include <Angel.h>

#include <vector>
#include <stdio.h>

#include "ModernMagic/ModernMagic.h"
#include "ModernMagic/DynamicProperty.h"

using namespace std;
using namespace ModernMagic;

#define theGameManager MainGameManager::GetInstance()

const Vector2 kRoleSpeed (6, 6);
const Vector2 kSlowRoleSpeed (2, 2);

class TestMessageListener : public MessageListener
{
public:
    virtual void ReceiveMessage(Message* m)
    {
        cout << m->GetMessageName() << endl;
    }
};

MM_DEF_BODY_TEYP_BEGIN(TestBulletType, Bullet, "TestBullet")

TestBulletType():
    tid(GetTextureReference("./Resources/Images/bulwhi.png"))
{
    MM_ASSERT(tid != -1);
}

virtual void OnApplying(Bullet *body) override
{
    body->SetRenderRadius(0.5f);
    body->SetRadius(0.2f);
    body->SetTexture(tid);
    body->SetColor(Color::FromHexString("#FF0000"));
}

private:
    int tid;

MM_DEF_BODY_TYPE_END

MM_DEF_BODY_TEYP_BEGIN(TestLaserType, Laser, "TestLaser")

TestLaserType():
    tid(GetTextureReference("./Resources/Images/stars-grayscale.png", GL_CLAMP, GL_LINEAR, false))
{
    MM_ASSERT(tid != -1);
}

virtual void OnApplying(Laser *body) override
{
    body->SetRadius(0.3f);
    body->SetTexture(tid);
}

private:
    int tid;

MM_DEF_BODY_TYPE_END

class MainGameManager : public GameManager
{
private:
    Actor *_role;
    bool _isDeath;
    AnnularEmitter *emitter;
    Bullet *probody;
    
    void InitMainTimeline()
    {
        Timeline &mtl = theMMMainTimeline;
        
        emitter = mtl.Add(new AnnularEmitter());
        probody = new Bullet();
        probody->SetSpeed(3.0f);
        probody->SetType("TestBullet");
        probody->SetPosition(Vector2(0, 7));
        
        emitter->SetPrototype(probody);
        emitter->SetAddiAngle(0.0f);
        emitter->SetWayNumber(8);
        emitter->SetInterval(0.05f);
        emitter->SetEmittedNumber(-1);
        
        auto emitterBody = new LoopTimeline(-1);
        emitterBody->SetBindedObject(probody);
        
        emitterBody->Add(new PropertyAnimationBy<float>(
            "Angle", -360, 6.0f, IntervalType::Smooth
        ));
        emitterBody->Add(new PropertyAnimationBy<float>(
            "Angle", 360, 6.0f, IntervalType::Smooth
        ));
        theMMActionUpdater.Add(emitterBody);
        
        auto followTl = new LoopTimeline(-1);
        followTl->Add(new ActionSleep(3.0f));
        followTl->Add(new ActionCallback(
            [&, this](ActionCallback*) {
                auto newtl = new Timeline();
                newtl->SetBindedObject(probody);
                
                newtl->Add(new PropertyAnimationTo<Vector2>(
                    "Position", _role->GetPosition(), 3.0f,
                    IntervalType::ExponentialInOut
                ));
                theMMActionUpdater.Add(newtl);
            }
        ));
        theMMActionUpdater.Add(followTl);
    }

    MainGameManager():
        _isDeath(false)
    {
        Bullet::TypeManager().Register<TestBulletType>();
        Laser::TypeManager().Register<TestLaserType>();

        RegisterFont("Resources/Fonts/Inconsolata.otf", 30, "Inconsolata");
        RegisterFont("Resources/Fonts/Inconsolata.otf", 100, "Big Inconsolata");

        _role = new Actor();

        _role->SetName("ForDemoConsole");
        _role->SetColor(0.0f, 1.0f, 0.5f);
        _role->SetDrawShape(ADS_Circle);
        _role->SetSize(0.5f);

        theWorld.Add(_role);
        
        this->InitMainTimeline();

        theMMEngine.SetWorldBox(BoundingBox(Vector2(-14, -10), Vector2(14, 10)));
        theMMEngine.Start();
        theSound.SetSoundCallback(this, &GameManager::SoundEnded);
        //auto sample = theSound.LoadSample("Resources/Sounds/Game Of Thrones Main Title.mp3", false);
        //theSound.PlaySound(sample, 1.0f, true);
    }

    size_t GetFPS(float dt)
    {
        static float time = 0.0f, lastTime = 0.0f;
        static size_t fps, finalfps;

        time += dt;

        if (time - lastTime > 1.0f)
        {
            lastTime = time;
            finalfps = fps;
            fps = 0;
        }

        ++fps;
        return finalfps;
    }

public:
    static MainGameManager &GetInstance()
    {
        static auto manager = new MainGameManager();
        return *manager;
    }

    virtual void Render() override
    {
        theMMEngine.Render();

        glColor3f(0.0f, 0.5f, 0.5f);

        DrawGameText (
            string("Bullets: ").append(to_string(theMMEngine.BodyUpdater().GetObjects().size())),
            "Inconsolata", 770, 730
            );

        DrawGameText (
            string("FPS: ").append(to_string(this->GetFPS(theWorld.GetDT()))),
            "Inconsolata", 770, 755
            );
        DrawGameText (
            string("Angle: ").append(to_string(probody->GetAngle())),
            "Inconsolata", 770, 700
            );
    }

    virtual void Update(float dt) override
    {
        if (_isDeath) return;
        
        bool isChangedRole = false;
        const Vector2 &rolePos = _role->GetPosition();
        Vector2 speedAppliedDt;
        
        if (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS) {
            speedAppliedDt = kSlowRoleSpeed * dt;
        } else {
            speedAppliedDt = kRoleSpeed * dt;
        }

        if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS)
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X, rolePos.Y + speedAppliedDt.Y);
        }
        else if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X, rolePos.Y - speedAppliedDt.Y);
        }

        if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X - speedAppliedDt.X, rolePos.Y);
        }
        else if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X + speedAppliedDt.X, rolePos.Y);
        }

        if (isChangedRole) {
            
        }
        
        /*
        for (auto each : theMMBodyUpdater.GetObjects()) {
            auto body = static_cast<Body*>(each);
            if (body->IsCollided(_role->GetPosition(), 0)) {
                _isDeath = true;
                _role->SetColor(1, 0, 0);
                TextActor *text = new TextActor();
                
                text->SetColor(1, 0, 0);
                text->SetFont("Big Inconsolata");
                text->SetDisplayString("Game Over!");
                text->SetPosition(theCamera.GetSize() / 2);
                text->SetAlignment(TextAlignment::TXT_Center);
                
                theWorld.Add(text);
            }
        }
        */
        //_rb->SetSegment(Segment::Rotate(_rb->GetSegment(), MathUtil::PiOver2 * dt));

        theMMEngine.Update(dt);
    }

    virtual void SoundEnded(AngelSoundHandle sound) override
    {

    }
};

int main(int argc, char* argv[])
{
    theWorld.Initialize(1024, 768, "ModernMagic Engine", true, false);

    //adds the default grid so you can more easily place Actors
    theWorld.Add(new GridActor(), -1);

    //YOUR GAME SETUP CODE HERE

    theWorld.SetGameManager(&theGameManager);

    // do all your setup first, because this function won't return until you're exiting
    theWorld.StartGame();

    // any cleanup can go here
    theWorld.Destroy();

    return 0;
}