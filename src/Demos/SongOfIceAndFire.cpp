#include "stdafx.h"

#include <Angel.h>

#include <vector>
#include <stdio.h>

#include "ModernMagic/ModernMagic.h"

using namespace std;
using namespace ModernMagic;

#define theGameManager MainGameManager::GetInstance()

const Vector2 kRoleSpeed (5, 5);

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
    body->SetRadius(0.5f);
    body->SetTexture(tid);
    body->SetColor(Color::FromHexString("#FF0000"));
    body->SetOpacity(0.5f);
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

void InitBodyTimeline(Body *body)
{
    Timeline *tl = new Timeline();
    body->SetAppliedTimeline(tl);
    
    auto mgr = tl->Add(new ActionGroup());
    
    mgr->Add(new ChangeSpeedTo(4.0f, 0.4f, IT_ExponentialOut));
    mgr->Add(new ChangeColorTo(Color::FromHexString("#7EC0EE"), 2.0f, IT_SineOut));
    
    tl->Add(new ChangeAngleAddiTo(50.0f));
    
    tl->Add(new ActionSleep(2.0f));
    tl->Add(new ChangeAngleAddiTo(25.0f));
    
    mgr = tl->Add(new ActionGroup());
    
    Timeline *newTl = new Timeline();
    newTl->Add(new ActionSleep(1.0f));
    newTl->Add(new ChangeColorTo(Color::FromHexString("#FF0000"), 1.0f, IT_Smooth));
    
    mgr->Add(newTl);
    mgr->Add(new MoveTo(Vector2::Zero, 2.0f, IT_ExponentialOut));
    
    tl->Add(new ChangeSpeedTo(0.2f));
}

class MainGameManager : public GameManager
{
private:
    Actor *_role;
    TextActor *_text;
    size_t accum;
    
    vector<ReboundBoard*> _rbs;
    
    MainGameManager():
    accum(0)
    {
        Bullet::TypeManager().Register<TestBulletType>();
        Laser::TypeManager().Register<TestLaserType>();
        
        RegisterFont("Resources/Fonts/Inconsolata.otf", 30, "Inconsolata");
        
        _role = new Actor();
        
        _role->SetName("ForDemoConsole");
        _role->SetColor(0.0f, 1.0f, 0.5f);
        _role->SetDrawShape(ADS_Circle);
        _role->SetSize(0.5f);
        
        theWorld.Add(_role);
        
        theMMMainTimeline.Add(new ActionSleep(1.0f));
        
        auto loopTl = theMMMainTimeline.Add(new LoopTimeline(-1));
        
        auto emitter = loopTl->Add(new AnnularEmitter());
        
        Bullet *_refbody = new Bullet();
        
        _refbody->SetSpeed(15.0f);
        _refbody->SetAngleAddition(-20.0f);
        _refbody->SetType("TestBullet");
        _refbody->SetIsDestroyWhenMoveOffScreen(false);
        
        InitBodyTimeline(_refbody);
        
        emitter->SetRefBody(_refbody);
        emitter->SetInterval(0.1f);
        emitter->SetEmittedTimes(60);
        emitter->SetAddiAngle(5.0f);
        emitter->SetWayNumber(30);
        
        loopTl->Add(new ActionSleep(6.5f));
        loopTl->Add (
                     new CollisionSelector(new RoundCollider(Vector2::Zero, 15))
                     )
        ->SetOnSelected([&](IBodySelector*, Body *body) {
            if (MathUtil::RandomInt(6) != 1) {
                body->Uninstall();
                return;
            }
            
            body->SetAngleAddition(10.0f);
            MM_PRO_BIN_OPER_PTR(body, Angle, +, -20 + MathUtil::RandomInt(40));
            
            body->SetIsDestroyWhenTimelineEnd(true);
            
            Timeline *tl = body->GetAppliedTimeline();
            tl->Clear();
            
            tl->Add(new ChangeSpeedBy(10.0f));
            tl->Add(new ActionSleep(2.0f));
            
            tl->Add(new MoveTo(Vector2::Zero, 1.0f, IT_ExponentialOut));
            tl->Add(new ChangeSpeedTo(5 + MathUtil::RandomInt(15)));
            
            tl->Add(new ChangeOpacityTo(0.0f, 2.0f, IT_ExponentialInOut));
            
            tl->Install();
        });
        
        theMMEngine.SetWorldBox(BoundingBox(Vector2(-14, -10), Vector2(14, 10)));
        
        theMMEngine.Start();
        
        theSound.SetSoundCallback(this, &GameManager::SoundEnded);
        auto sample = theSound.LoadSample("Resources/Sounds/3rd eye.mp3", false /*no stream*/);
        theSound.PlaySound(sample, 1.0f, true);
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
        
        for (auto rb : _rbs)
            rb->GetSegment().DebugDraw();
        
        DrawGameText (
                      string("Bullets: ").append(to_string(theMMEngine.BodyUpdater().GetObjects().size())),
                      "Inconsolata", 770, 730
                      );
        
        DrawGameText (
                      string("FPS: ").append(to_string(this->GetFPS(theWorld.GetDT()))),
                      "Inconsolata", 770, 755
                      );
        
        DrawGameText(
                     string("Collided: ").append(to_string(accum)),
                     "Inconsolata", 770, 705
                     );
    }
    
    virtual void Update(float dt) override
    {
        bool isChangedRole = false;
        const Vector2 &rolePos = _role->GetPosition();
        Vector2 speedAppliedDt = kRoleSpeed * dt;
        
        if (theInput.IsKeyDown('w'))
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X, rolePos.Y + speedAppliedDt.Y);
        }
        else if (theInput.IsKeyDown('s'))
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X, rolePos.Y - speedAppliedDt.Y);
        }
        
        if (theInput.IsKeyDown('a'))
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X - speedAppliedDt.X, rolePos.Y);
        }
        else if (theInput.IsKeyDown('d'))
        {
            isChangedRole = true;
            _role->SetPosition(rolePos.X + speedAppliedDt.X, rolePos.Y);
        }
        
        if (isChangedRole) {
            
        }
        
        accum = 0;
        
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