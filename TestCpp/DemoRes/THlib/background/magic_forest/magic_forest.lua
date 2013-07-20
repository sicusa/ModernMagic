magic_forest_background=Class(background)

function magic_forest_background:init()
	background.init(self,false)
	LoadTexture('magic_forest_ground','THlib\\background\\magic_forest\\ground.png')
	LoadImage('magic_forest_ground','magic_forest_ground',0,0,512,512)
	LoadTexture('magic_forest_mask','THlib\\background\\magic_forest\\mask.png')
	LoadImage('magic_forest_mask','magic_forest_mask',0,0,256,256)
	Set3D('z',0,3)
	Set3D('eye',0.25,-2.2,-1.1)
	Set3D('at',0.25,0,0)
	Set3D('fovy',0.4)
	Set3D('fog',2.5,4,Color(0x00000000))
	self.yos=0
	self.speed=0.004
end

function magic_forest_background:frame()
	self.yos=self.yos+self.speed
end

function magic_forest_background:render()
	SetViewMode'3d'
	RenderClear(lstg.view3d.fog[3])
	local y=self.yos%2
	Render4V('magic_forest_ground',0,1-y,0,1,-y,0,0,-1-y,0,-1,-y,0)
	Render4V('magic_forest_ground',1,2-y,0,2,1-y,0,1,-y,0,0,1-y,0)
	Render4V('magic_forest_ground',1,-y,0,2,-1-y,0,1,-2-y,0,0,-1-y,0)
	Render4V('magic_forest_ground',0,3-y,0,1,2-y,0,0,1-y,0,-1,2-y,0)
	Render4V('magic_forest_ground',2,1-y,0,3,-y,0,2,-1-y,0,1,-y,0)
	Render4V('magic_forest_ground',-1,2-y,0,0,1-y,0,-1,-y,0,-2,1-y,0)
	Render4V('magic_forest_ground',2,3-y,0,3,2-y,0,2,1-y,0,1,2-y,0)
	Render4V('magic_forest_ground',0,5-y,0,1,4-y,0,0,3-y,0,-1,4-y,0)
	Render4V('magic_forest_ground',1,4-y,0,2,3-y,0,1,2-y,0,0,3-y,0)
	Render4V('magic_forest_ground',-1,4-y,0,0,3-y,0,-1,2-y,0,-2,3-y,0)
	--
	Render4V('magic_forest_mask',0,1-y,0.2,1,-y,0.2,0,-1-y,0.2,-1,-y,0.2)
	Render4V('magic_forest_mask',1,2-y,0.2,2,1-y,0.2,1,-y,0.2,0,1-y,0.2)
	Render4V('magic_forest_mask',1,-y,0.2,2,-1-y,0.2,1,-2-y,0.2,0,-1-y,0.2)
	Render4V('magic_forest_mask',0,3-y,0.2,1,2-y,0.2,0,1-y,0.2,-1,2-y,0.2)
	Render4V('magic_forest_mask',2,1-y,0.2,3,-y,0.2,2,-1-y,0.2,1,-y,0.2)
	Render4V('magic_forest_mask',-1,2-y,0.2,0,1-y,0.2,-1,-y,0.2,-2,1-y,0.2)
	Render4V('magic_forest_mask',2,3-y,0.2,3,2-y,0.2,2,1-y,0.2,1,2-y,0.2)
	Render4V('magic_forest_mask',0,5-y,0.2,1,4-y,0.2,0,3-y,0.2,-1,4-y,0.2)
	Render4V('magic_forest_mask',1,4-y,0.2,2,3-y,0.2,1,2-y,0.2,0,3-y,0.2)
	Render4V('magic_forest_mask',-1,4-y,0.2,0,3-y,0.2,-1,2-y,0.2,-2,3-y,0.2)
	SetViewMode'world'
end