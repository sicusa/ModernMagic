background=Class(object)

function background:init(is_sc_bg)
	self.group=GROUP_GHOST
	if is_sc_bg then
		self.layer=LAYER_BG
		self.alpha=0
	else
		self.layer=LAYER_BG-1
		self.alpha=1
		lstg.tmpvar.bg=self
	end
end

function background:render()
	SetViewMode'world'
	SetImageState('white','',Color(self.alpha*255,0,0,0))
	RenderRect('white',-224,224,-256,256)
end

camera_setter=Class(object)

function camera_setter:init()
	self.hide=true
	self.group=GROUP_GHOST
end

function camera_setter:frame()
	if KeyIsDown'shoot' then
		if KeyIsDown'up'    then lstg.view3d.eye[3]=lstg.view3d.eye[3]+0.01 end
		if KeyIsDown'down'  then lstg.view3d.eye[3]=lstg.view3d.eye[3]-0.01 end
	elseif KeyIsDown'spell' then
	
	else
		if KeyIsDown'up'    then lstg.view3d.eye[2]=lstg.view3d.eye[2]+0.01 end
		if KeyIsDown'down'  then lstg.view3d.eye[2]=lstg.view3d.eye[2]-0.01 end
		if KeyIsDown'left'  then lstg.view3d.eye[1]=lstg.view3d.eye[1]+0.01 end
		if KeyIsDown'right' then lstg.view3d.eye[1]=lstg.view3d.eye[1]-0.01 end
	end
	if KeyIsPressed'special' then print(string.format('Set3D(\'eye\',%f,%f,%f)',lstg.view3d.eye[1],lstg.view3d.eye[2],lstg.view3d.eye[3])) end
end

Include'THlib\\background\\bamboo\\bamboo.lua'
Include'THlib\\background\\magic_forest\\magic_forest.lua'