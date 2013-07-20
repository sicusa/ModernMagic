marisa_player=Class(player)

function marisa_player:init()
	LoadTexture('marisa_player','THlib\\player\\marisa\\marisa.png')
	LoadImageGroup('marisa_player','marisa_player',0,0,32,48,8,3,0,0)
	LoadImage('marisa_bullet','marisa_player',0,144,32,16,16,16)
	LoadAnimation('marisa_bullet_ef','marisa_player',0,144,32,16,4,1,4)
	SetImageState('marisa_bullet','',Color(0x80FFFFFF))
	LoadImage('marisa_support','marisa_player',144,144,16,16)
	LoadImage('marisa_laser_light','marisa_player',224,224,32,32)
	SetImageState('marisa_laser_light','mul+add',Color(0xFFFFFFFF))
	LoadPS('marisa_sp_ef','THlib\\player\\marisa\\marisa_sp_ef.psi','parimg6')
	for i=1,8 do
		LoadTexture('marisa_face_'..i,'THlib\\player\\marisa\\marisa_face_'..i..'.png')
		LoadImage('marisa_face_'..i,'marisa_face_'..i,0,1,256,319)
	end
	player.init(self)
	self.imgs={}
	for i=1,24 do self.imgs[i]='marisa_player'..i end
	self.hspeed=5
	self.lspeed=2.5
	self.slist=
	{
		{            nil,          nil,              nil,            nil},
		{{  0,32,  0,29},          nil,              nil,            nil},
		{{-30,10, -8,23},{30,10, 8,23},              nil,            nil},
		{{-30,10,-15,20},{30,10,15,20},{   0,32,   0,29},            nil},
		{{-30,10,-15,20},{30,10,15,20},{-7.5,32,-7.5,29},{7.5,32,7.5,29}},
		{{-30,10,-15,20},{30,10,15,20},{-7.5,32,-7.5,29},{7.5,32,7.5,29}},
	}
end

function marisa_player:shoot()
	self.nextshoot=4
	New(marisa_bullet,'marisa_bullet',self.x+6,self.y,24,PI_2,1.25)
	New(marisa_bullet,'marisa_bullet',self.x-6,self.y,24,PI_2,1.25)
	if self.fire==1 then
		for i=1,4 do if self.sp[i] and self.sp[i][3]>0.5 then
			CreateMarisaLaser(self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],PI_2,0.125)
		end end
	end
end

function marisa_player:spell()
	PlaySound('slash',1.0)
	PlaySound('nep00',1.0)
	New(marisa_sp_ef,0)
	New(marisa_sp_ef,0)
	New(marisa_sp_ef,PIx2/3)
	New(marisa_sp_ef,PIx2/3)
	New(marisa_sp_ef,2*PIx2/3)
	New(marisa_sp_ef,2*PIx2/3)
	misc.ShakeScreen(240,10)
	New(tasker,function()
		New(bullet_killer,self.x,self.y)
		local rot=0
		for i=1,120 do
			rot=rot+0.04
			for i=1,3 do
				local a=rot+PI+i*PIx2/3
				New(player_bullet_hide,16,16,self.x,self.y,12,a,0.4)
				for j=1,8 do
					New(player_bullet_hide,16,16,self.x,self.y,12,a+0.0375*j,0.4,2*j)
					New(player_bullet_hide,16,16,self.x,self.y,12,a-0.0375*j,0.4,2*j)
				end
			end
			task.Wait(2)
		end
		PlaySound('slash',1.0)
		New(bullet_killer,self.x,self.y)
	end)
	self.nextspell=240
	self.protect=360
end

function marisa_player:render()
	local offset=(self.timer*4-self.y)%256
	local sz=1.2+0.1*sin(self.timer*0.2)
	--support
	SetImageState('marisa_support','',Color(0xFFFFFFFF))
	for i=1,4 do if self.sp[i] then
		Render('marisa_support',self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],0,self.sp[i][3],1)
	end end
	--support deco
	SetImageState('marisa_support','',Color(0x80FFFFFF))
	for i=1,4 do if self.sp[i] then
		Render('marisa_support',self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],0,self.sp[i][3]*sz,sz)
	end end
	--laser
	if int(self.support)==5 then
		for i=1,4 do if self.sp[i] then
			RenderMarisaLaser(self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],offset,8*self.fire*self.sp[i][3],Color(0xC0FFFF80))
		end end
	else
		for i=1,4 do if self.sp[i] then
			RenderMarisaLaser(self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],offset,8*self.fire*self.sp[i][3],Color(0x80FFFFFF))
		end end
	end
	--laser light
	for i=1,4 do if self.sp[i] then
		Render('marisa_laser_light',self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],self.ani*0.1,self.sp[i][3]*sz*self.fire*0.8)
	end end
	player.render(self)
end

marisa_bullet=Class(player_bullet_straight)

function marisa_bullet:kill()
	New(marisa_bullet_ef,self.x,self.y,self.rot,3)
	New(marisa_bullet_ef,self.x,self.y,self.rot,4)
	New(marisa_bullet_ef,self.x,self.y,self.rot,5)
end

marisa_laser=Class(object)

function marisa_laser:init(x,y,dmg)
	self.x=x self.y=y
	self.group=GROUP_PLAYER_BULLET
	self.dmg=dmg
	self.hide=true
	self.a=16 self.b=16
end

function marisa_laser:frame()
	if self.timer==3 then Del(self) end
end

function CreateMarisaLaser(x,y,angle,dmg)
	local dx=8*cos(angle)
	local dy=8*sin(angle)
	for i=0,75 do
		New(marisa_laser,x+i*dx,y+i*dy,dmg)
	end
end

function RenderMarisaLaser(x,y,offset,w,c)
	local l1=offset
	local l2=256-offset
	RenderTexture('marisa_player','mul+add',
				{x-w,y   ,0.5, l2,176,c},
				{x-w,y+l1,0.5,256,176,c},
				{x+w,y+l1,0.5,256,192,c},
				{x+w,y   ,0.5, l2,192,c}
				)
	RenderTexture('marisa_player','mul+add',
				{x-w,y+l1    ,0.5,  0,176,c},
				{x-w,y+l1+256,0.5,256,176,c},
				{x+w,y+l1+256,0.5,256,192,c},
				{x+w,y+l1    ,0.5,  0,192,c}
				)
	RenderTexture('marisa_player','mul+add',
				{x-w,y+l1+256,0.5,  0,176,c},
				{x-w,y+l1+512,0.5,256,176,c},
				{x+w,y+l1+512,0.5,256,192,c},
				{x+w,y+l1+256,0.5,  0,192,c}
				)
end

marisa_bullet_ef=Class(object)

function marisa_bullet_ef:init(x,y,rot,v)
	self.x=x
	self.y=y
	self.rot=rot
	self.vx=v*cos(rot)
	self.vy=v*sin(rot)
	self.img='marisa_bullet_ef'
	self.layer=LAYER_PLAYER_BULLET+50
end

function marisa_bullet_ef:frame()
	if self.timer==16 then Del(self) end
end
function marisa_bullet_ef:render()
	SetAnimationState('marisa_bullet_ef','',Color(128-8*self.timer,255,255,255))
	object.render(self)
end

marisa_sp_ef=Class(object)

function marisa_sp_ef:init(rot)
	self.layer=LAYER_PLAYER-1
	self.rot=rot
	self.img='marisa_sp_ef'
end

function marisa_sp_ef:frame()
	self.x=lstg.player.x
	self.y=lstg.player.y
	self.rot=self.rot+0.02
	if self.timer>240 then ParticleSetEmission(self,ParticleGetEmission(self)-10) end
	if self.timer==480 then Del(self) end
end
