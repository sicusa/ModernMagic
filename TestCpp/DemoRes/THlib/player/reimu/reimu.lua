reimu_player=Class(player)

function reimu_player:init()
	LoadTexture('reimu_player','THlib\\player\\reimu\\reimu.png')
	LoadImageGroup('reimu_player','reimu_player',0,0,32,48,8,3,0,0)
	LoadImage('reimu_bullet_red','reimu_player',0,176,64,16,16,16)
	SetImageState('reimu_bullet_red','',Color(0x80FFFFFF))
	SetImageCenter('reimu_bullet_red',56,8)
	LoadImage('reimu_bullet_blue','reimu_player',0,160,16,16,16,16)
	SetImageState('reimu_bullet_blue','',Color(0x80FFFFFF))
	LoadImage('reimu_support','reimu_player',64,144,16,16)
	LoadImage('reimu_bullet_ef_img','reimu_player',48,144,16,16)
	LoadPS('reimu_bullet_ef','THlib\\player\\reimu\\reimu_bullet_ef.psi','reimu_bullet_ef_img')
	LoadPS('reimu_sp_ef','THlib\\player\\reimu\\reimu_sp_ef.psi','parimg1',16,16)
	for i=1,8 do
		LoadTexture('reimu_face_'..i,'THlib\\player\\reimu\\reimu_face_'..i..'.png')
		LoadImage('reimu_face_'..i,'reimu_face_'..i,0,1,256,319)
	end
	player.init(self)
	self.imgs={}
	for i=1,24 do self.imgs[i]='reimu_player'..i end
	self.slist=
	{
		{              nil,            nil,               nil,             nil},
		{{  0,-32,  0,-32},            nil,               nil,             nil},
		{{-30,-10,-15,-20},{30,-10,15,-20},               nil,             nil},
		{{-30,-10,-15,-20},{30,-10,15,-20},{  0,-32,   0,-32},             nil},
		{{-30,-10,-15,-20},{30,-10,15,-20},{-15,-32,-7.5,-32},{15,-32,7.5,-32}},
		{{-30,-10,-15,-20},{30,-10,15,-20},{-15,-32,-7.5,-32},{15,-32,7.5,-32}},
	}
end

function reimu_player:shoot()
	self.nextshoot=4
	New(reimu_bullet_red,'reimu_bullet_red',self.x+6,self.y,24,PI_2,1)
	New(reimu_bullet_red,'reimu_bullet_red',self.x-6,self.y,24,PI_2,1)
	if self.timer%8<4 then
		for i=1,4 do if self.sp[i] and self.sp[i][3]>0.5 then
			New(reimu_bullet_blue,'reimu_bullet_blue',self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],12,PI_2,self.target,60,1.25)
		end end
	end
end

function reimu_player:spell()
	PlaySound('slash',1.0)
	PlaySound('nep00',1.0)
	misc.ShakeScreen(240,10)
	New(tasker,function()
		New(bullet_killer,self.x,self.y)
		for i=1,8 do
			task.Wait(15)
			local n=0
			local tar1,tar2
			for _,o in ObjList(GROUP_ENEMY) do if o.colli then n=n+1 end end
			if n==0 then tar1=nil tar2=nil else
				local n1=(643*i)%n+1
				local n2=(997*i)%n+1
				n=0
				for _,o in ObjList(GROUP_ENEMY) do
					if o.colli then n=n+1 end
					if n==n1 then tar1=o end
					if n==n2 then tar2=o end
				end
			end
			New(reimu_sp_ef,'reimu_sp_ef',self.x+64*cos(PI_2+(i-0.5)*PI/8),self.y+64*sin(PI_2+(i-0.5)*PI/8),5,PI_2+(i-0.5)*PI/8,tar1,15,10)
			New(reimu_sp_ef,'reimu_sp_ef',self.x+64*cos(PI_2-(i-0.5)*PI/8),self.y+64*sin(PI_2-(i-0.5)*PI/8),5,PI_2-(i-0.5)*PI/8,tar2,15,10)
		end
		task.Wait(120)
		PlaySound('slash',1.0)
		New(bullet_killer,self.x,self.y)
	end)
	self.nextspell=240
	self.protect=360
end

function reimu_player:render()
	local rot=self.timer*0.1
	if int(self.support)==5 then
		SetImageState('reimu_support','',Color(0x80FFFFFF))
		for i=1,4 do if self.sp[i] then
			Render('reimu_support',self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],-rot,self.sp[i][3]*1.3,1.3)
		end end
		SetImageState('reimu_support','',Color(0xFFFFFFFF))
	end
	for i=1,4 do if self.sp[i] then
		Render('reimu_support',self.supportx+self.sp[i][1],self.supporty+self.sp[i][2],rot,self.sp[i][3],1)
	end end
	player.render(self)
end

reimu_bullet_red=Class(player_bullet_straight)

function reimu_bullet_red:kill()
	New(reimu_bullet_ef,self.x,self.y,self.rot+PI)
end

reimu_bullet_blue=Class(player_bullet_trail)

function reimu_bullet_blue:kill()
	New(reimu_bullet_ef,self.x,self.y,self.rot+PI)
end

reimu_sp_ef=Class(player_bullet_trail)

function reimu_sp_ef:kill()
	PlaySound('explode',0.3)
	New(bubble,'parimg12',self.x,self.y,30,4,6,Color(0xFFFFFFFF),Color(0x00FFFFFF),LAYER_ENEMY_BULLET_EF,'')
	for i=1,16 do
		New(reimu_sp_ef2,16,16,self.x,self.y,3,PIx2/16*i,0.25,4,30)
	end
	misc.KeepParticle(self)
end

function reimu_sp_ef:del()
	misc.KeepParticle(self)
end

reimu_bullet_ef=Class(object)

function reimu_bullet_ef:init(x,y,rot)
	self.x=x self.y=y self.rot=rot self.img='reimu_bullet_ef' self.layer=LAYER_PLAYER_BULLET+50
end

function reimu_bullet_ef:frame()
	if self.timer==4 then ParticleStop(self) end
	if self.timer==30 then Del(self) end
end

reimu_sp_ef2=Class(object)

function reimu_sp_ef2:init(a,b,x,y,v,angle,dmg,delay,life)
	self.group=GROUP_PLAYER_BULLET
	self.layer=LAYER_PLAYER_BULLET
	self.colli=false
	self.a=a
	self.b=b
	self.x=x
	self.y=y
	self.rot=angle
	self.vx=v*cos(angle)
	self.vy=v*sin(angle)
	self.dmg=dmg
	self.delay=delay or 0
	self.life=life
	self.hide=true
end

function reimu_sp_ef2:frame()
	if self.timer==self.delay then self.colli=true end
	if self.timer==self.life then Del(self) end
end
