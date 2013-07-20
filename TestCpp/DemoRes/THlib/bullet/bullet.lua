LoadTexture('bullet1','THlib\\bullet\\bullet1.png',true)
LoadImageGroup('preimg','bullet1',80,0,32,32,1,8)
LoadImageGroup('arrow_big','bullet1',0,0,16,16,1,16,4,4)
LoadImageGroup('gun_bullet','bullet1',24,0,16,16,1,16,4,3)
LoadImageGroup('gun_bullet_void','bullet1',56,0,16,16,1,16,4,3)
LoadImageGroup('butterfly','bullet1',112,0,32,32,1,8,4,4)
LoadImageGroup('square','bullet1',152,0,16,16,1,16,4,4)
LoadImageGroup('ball_mid','bullet1',176,0,32,32,1,8,4,4)
LoadImageGroup('mildew','bullet1',208,0,16,16,1,16,3,3)
LoadImageGroup('ellipse','bullet1',224,0,32,32,1,8,7,5)

LoadTexture('bullet2','THlib\\bullet\\bullet2.png')
LoadImageGroup('star_small','bullet2',96,0,16,16,1,16,3,3)
LoadImageGroup('star_big','bullet2',224,0,32,32,1,8,6,6)
LoadImageGroup('ball_huge','bullet2',0,0,64,64,1,4,20,20)
LoadImageGroup('fade_ball_huge','bullet2',0,0,64,64,1,4,20,20)
LoadImageGroup('ball_big','bullet2',192,0,32,32,1,8,8,8)
LoadImageGroup('ball_small','bullet2',176,0,16,16,1,16,2,2)
LoadImageGroup('grain_a','bullet2',160,0,16,16,1,16,3,3)
LoadImageGroup('grain_b','bullet2',128,0,16,16,1,16,3,3)

LoadTexture('bullet3','THlib\\bullet\\bullet3.png')
LoadAnimation('water_drop1','bullet3',176,0,32,32,1,4,4,6,6)
LoadAnimation('water_drop2','bullet3',176,128,32,32,1,4,4,6,6)
LoadAnimation('water_drop3','bullet3',224,0,32,32,1,4,4,6,6)
LoadAnimation('water_drop4','bullet3',224,128,32,32,1,4,4,6,6)
SetAnimationState('water_drop1','mul+add')
SetAnimationState('water_drop2','mul+add')
SetAnimationState('water_drop3','mul+add')
SetAnimationState('water_drop4','mul+add')
LoadImageGroup('knife','bullet3',0,0,32,32,1,8,12,3)
LoadImageGroup('grain_c','bullet3',48,0,16,16,1,16,3,3)
LoadImageGroup('arrow_small','bullet3',80,0,16,16,1,16,3,3)
LoadImageGroup('kite','bullet3',112,0,16,16,1,16,3,3)

bullet=Class(object)
function bullet:init(imgclass,index,stay)
	self.logclass=self.class
	self.imgclass=imgclass
	self.class=imgclass
	self.group=GROUP_ENEMY_BULLET
	self.colli=false
	if stay then
		self._vx=self.vx self._vy=self.vy self._omiga=self.omiga
		self.vx=0 self.vy=0 self.omiga=0
	end
	imgclass.init(self,index,stay)
end

function bullet:kill()
	New(item_faith_minor,self.x,self.y)
	self.imgclass.del(self)
end
function bullet:del()
	self.imgclass.del(self)
end
----------------------------------------------------------------
img_class=Class(object)
function img_class:frame()
	if not self.stay then self.logclass.frame(self) end
	if self.timer==self.pretime then
		self.class=self.logclass
		self.layer=LAYER_ENEMY_BULLET-self.size+self.index*0.00001
		self.colli=true
		if self.stay then
			self.timer=-1
			self.vx=self._vx
			self.vy=self._vy
			self.omiga=self._omiga
		end
	end
end
function img_class:del()
	New(bubble2,'preimg'..self.index,self.x,self.y,self.dx,self.dy,self.posttime,self.size,0,Color(0xFFFFFFFF),Color(0xFFFFFFFF),self.layer,'mul+add')
end
function img_class:kill()
	New(item_faith_minor,self.x,self.y)
	img_class.del(self)
end
function img_class:render()
	SetImageState('preimg'..self.index,'',Color(255*self.timer/self.pretime,255,255,255))
	Render('preimg'..self.index,self.x,self.y,self.rot,((self.pretime-self.timer)/self.pretime*3+1)*self.size)
end
----------------------------------------------------------------
arrow_big=Class(img_class)
function arrow_big:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='arrow_big'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.6
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
gun_bullet=Class(img_class)
function gun_bullet:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='gun_bullet'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.4
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
butterfly=Class(img_class)
function butterfly:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='butterfly'..int((index+1)/2)
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.7
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
square=Class(img_class)
function square:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='square'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.8
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
ball_mid=Class(img_class)
function ball_mid:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='ball_mid'..int((index+1)/2)
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.75
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
mildew=Class(img_class)
function mildew:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='mildew'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.401
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
ellipse=Class(img_class)
function ellipse:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='ellipse'..int((index+1)/2)
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.701
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
star_small=Class(img_class)
function star_small:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='star_small'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.5
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
star_big=Class(img_class)
function star_big:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='star_big'..int((index+1)/2)
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.998
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
ball_huge=Class(img_class)
function ball_huge:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='ball_huge'..int((index+3)/4)
	self.index=int((index+1)/2)
	self.pretime=23
	self.posttime=12
	self.size=0.999
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
function ball_huge:render()
	SetImageState('fade_'..self.img,'',Color(255*self.timer/self.pretime,255,255,255))
	Render('fade_'..self.img,self.x,self.y,self.rot,(self.pretime-self.timer)/self.pretime+1)
end
function ball_huge:del()
	New(bubble2,'fade_'..self.img,self.x,self.y,self.dx,self.dy,self.posttime,1,0,Color(0xFFFFFFFF),Color(0x00FFFFFF),self.layer,'mul+add')
end
function ball_huge:kill()
	New(item_faith_minor,self.x,self.y)
	ball_huge.del(self)
end
----------------------------------------------------------------
ball_big=Class(img_class)
function ball_big:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='ball_big'..int((index+1)/2)
	self.index=int((index+1)/2)
	self.layer=LAYER_ENEMY_BULLET_EF
	self.pretime=11
	self.posttime=6
	self.size=1.0
end
----------------------------------------------------------------
ball_small=Class(img_class)
function ball_small:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='ball_small'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.402
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
grain_a=Class(img_class)
function grain_a:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='grain_a'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.403
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
grain_b=Class(img_class)
function grain_b:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='grain_b'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.404
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
grain_c=Class(img_class)
function grain_c:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='grain_c'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.405
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
kite=Class(img_class)
function kite:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='kite'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.406
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
knife=Class(img_class)
function knife:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='knife'..int((index+1)/2)
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.751
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
arrow_small=Class(img_class)
function arrow_small:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='arrow_small'..index
	self.index=int((index+1)/2)
	self.pretime=11
	self.posttime=6
	self.size=0.407
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
----------------------------------------------------------------
water_drop=Class(img_class)
function water_drop:init(index,stay)
	index=max(min(int(index),16),1)
	self.stay=stay
	self.img='water_drop'..int((index+3)/4)
	self.index=int((index+3)/4)*2-1
	self.pretime=11
	self.posttime=6
	self.size=0.702
	self.layer=LAYER_ENEMY_BULLET_EF-self.size+self.index*0.00001
end
function water_drop:render()
	SetImageState('preimg'..self.index,'mul+add',Color(255*self.timer/self.pretime,255,255,255))
	Render('preimg'..self.index,self.x,self.y,self.rot,((self.pretime-self.timer)/self.pretime*2+1)*self.size)
end
----------------------------------------------------------------

----------------------------------------------------------------
straight=Class(bullet)
function straight:init(imgclass,index,stay,x,y,v,angle,omiga)
	self.x=x self.y=y
	SetV(self,v,angle,true)
	self.omiga=omiga or 0
	bullet.init(self,imgclass,index,stay)
end
----------------------------------------------------------------
straight_495=Class(bullet)
function straight_495:init(imgclass,index,stay,x,y,v,angle,omiga)
	self.x=x self.y=y
	SetV(self,v,angle,true)
	self.omiga=omiga or 0
	bullet.init(self,imgclass,index,stay)
end
function straight_495:frame()
	if not self.reflected then
		if self.y> 224 then self.vy=-self.vy self.y= 448-self.y self.rot=  -self.rot self.reflected=true return end
		if self.x> 192 then self.vx=-self.vx self.x= 384-self.x self.rot=PI-self.rot self.reflected=true return end
		if self.x<-192 then self.vx=-self.vx self.x=-384-self.x self.rot=PI-self.rot self.reflected=true return end
	end
end
----------------------------------------------------------------
bullet_killer=Class(object)
function bullet_killer:init(x,y,kill_indes)
	self.x=x
	self.y=y
	self.group=GROUP_GHOST
	self.hide=true
	self.kill_indes=kill_indes
end
function bullet_killer:frame()
	if self.timer==40 then Del(self) end
	for i,o in ObjList(GROUP_ENEMY_BULLET) do
		if Dist(self,o)<self.timer*20 then Kill(o) end
	end
	if self.kill_indes then
		for i,o in ObjList(GROUP_INDES) do
			if Dist(self,o)<self.timer*20 then Kill(o) end
		end
	end
end
----------------------------------------------------------------
bullet_deleter=Class(object)
function bullet_deleter:init(x,y,kill_indes)
	self.x=x
	self.y=y
	self.group=GROUP_GHOST
	self.hide=true
	self.kill_indes=kill_indes
end
function bullet_deleter:frame()
	if self.timer==60 then Del(self) end
	for i,o in ObjList(GROUP_ENEMY_BULLET) do
		if Dist(self,o)<self.timer*20 then Del(o) end
	end
	if self.kill_indes then
		for i,o in ObjList(GROUP_INDES) do
			if Dist(self,o)<self.timer*20 then Del(o) end
		end
	end
end
