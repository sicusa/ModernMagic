LoopTimeline *lt = new LoopTimeline(-1);
theMMMainTimeline.Add(lt);

AnnularEmitter *emitter = new AnnularEmitter();
lt->Add(emitter);

Bullet *refBullet = new Bullet();

refBullet->SetSpeed(0.5f);
refBullet->SetType("TestBullet");
refBullet->SetColor(Color::FromHexString("#66CCFF"));

Timeline *bodyTl = new Timeline();
bodyTl->Add(new ChangeSpeedTo(5.0f, 0.6f, IT_ExponentialIn));
bodyTl->Add(new ActionSleep(3.0f));
bodyTl->Add(new ChangeOpacityTo(0.0f, 1.0f, IT_ExponentialOut));

refBullet->SetAppliedTimeline(bodyTl);
refBullet->SetIsDestroyWhenTimelineEnd(true);

emitter->SetRefBody(refBullet);
emitter->SetInterval(.1f);
emitter->SetEmittedTimes(8);
emitter->SetWayNumber(20);

lt->Add(new ActionCallback(
    [&, refBullet](ActionCallback*) {
        refBullet->SetPosition(
            MathUtil::RandomVector(
            theMMEngine.GetWorldBox().Min,
            theMMEngine.GetWorldBox().Max)
        );
    })
);

lt->Add(new ActionSleep(0.5f));

Segment segs[] = {
    Segment(-13,  10,  13,  10),
    Segment( 13,  10,  13, -10),
    Segment(-13,  10, -13, -10),
    Segment(-13, -10,  13, -10)
};

for (auto &each : segs) {
    ReboundBoard *rb = new ReboundBoard(each);
    _rbs.push_back(rb);
    theMMActionUpdater.Add(rb);
}