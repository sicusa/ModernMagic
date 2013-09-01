LoadTexture('enemy1','THlib\\enemy\\enemy1.png')
LoadImageGroup('girl1','enemy1',0,384,32,32,12,1,8,8)
LoadImageGroup('girl2','enemy1',0,416,32,32,12,1,8,8)
LoadImageGroup('girl3','enemy1',0,448,32,32,12,1,8,8)
LoadImageGroup('girl4','enemy1',0,480,32,32,12,1,8,8)
LoadImageGroup('girl5','enemy1',0,0,48,32,4,3,8,8)
LoadImageGroup('girl6','enemy1',0,96,48,32,4,3,8,8)
LoadImageGroup('girl7','enemy1',320,0,48,48,4,3,16,16)
LoadImageGroup('girl8','enemy1',320,144,48,48,4,3,16,16)
LoadImageGroup('girl9','enemy1',0,192,64,64,4,3,16,16)
LoadImageGroup('enemy_aura','enemy1',192,32,32,32,4,1)
for i=1,4 do SetImageState('enemy_aura'..i,'',Color(0x80FFFFFF)) end

enemy=Class(object)

function enemy:init(hp,death_ef)
	self.layer=LAYER_ENEMY
	self.group=GROUP_ENEMY
	self.bound=false
	self.colli=false
	self.maxhp=hp or 1
	self.hp=hp or 1
	self.death_ef=death_ef
	setmetatable(self,{__index=GetAttr,__newindex=enemy_meta_newindex})
	self.colli=true
end

function enemy_meta_newindex(t,k,v)
	if k=='colli' then rawset(t,'_colli',v)
	else SetAttr(t,k,v) end
end

function enemy:frame()
	SetAttr(self,'colli',self.x>lstg.world.l and self.x<lstg.world.r
	       and self.y>lstg.world.b and self.y<lstg.world.t
		   and self._colli)
	task.Do(self)
end

function enemy:colli(other)
	if other.dmg and (not self.protect) then
		self.hp=self.hp-other.dmg
		if self.hp<=0 then Kill(self) end
	end
	Kill(other)
	if not other.mute then
		if self.hp>self.maxhp*0.2 then PlaySound('damage00',0.4,self.x/200)
		else PlaySound('damage01',0.8,self.x/200) end
	end
end

function enemy:kill()
	New(enemy_death_ef,self.death_ef,self.x,self.y)
	if self.drop then item.DropItem(self.x,self.y,self.drop) end
end

girl=Class(enemy)

function girl:init(t,hp,death_ef)
	if t<=4 then death_ef=death_ef or t else death_ef=death_ef or 3 end
	enemy.init(self,hp,death_ef)
	self.type=t
	self.imgs={}
	for i=1,12 do self.imgs[i]='girl'..t..i end
	self.lr=1
end

function girl:frame()
	enemy.frame(self)
	if self.dx>0.5 then dx=1 elseif self.dx<-0.5 then dx=-1 else dx=0 end
	self.lr=self.lr+dx
	if self.lr> 18 then self.lr= 18 end
	if self.lr<-18 then self.lr=-18 end
	if self.lr==0 then self.lr=self.lr+dx end
	if dx==0 then
		if self.lr> 1 then self.lr=self.lr-1 end
		if self.lr<-1 then self.lr=self.lr+1 end
	end
	if abs(self.lr)==1 then
		self.img=self.imgs[int(self.ani/8)%4+1]
	elseif abs(self.lr)==18 then
		self.img=self.imgs[int(self.ani/8)%4+9]
	else
		self.img=self.imgs[int((abs(self.lr)-2)/4)+5]
	end
	self.hscale=sign(self.lr)
end

function girl:render()
	if self.type<=4 then Render('enemy_aura'..self.type,self.x,self.y,self.timer*0.1,1.2+0.2*sin(self.timer*0.15)) end
	object.render(self)
end

enemy_death_ef=Class(object)

function enemy_death_ef:init(index,x,y)
	self.img='bubble'..index
	self.layer=LAYER_ENEMY+50
	self.group=GROUP_GHOST
	self.x=x self.y=y self.rot=PI_4
	PlaySound('enep00',0.3,self.x/200)
end

function enemy_death_ef:render()
	local alpha=1-self.timer/30
	alpha=255*alpha^2
	SetImageState(self.img,'',Color(alpha,255,255,255))
	--Render(self.img,self.x,self.y,0,self.timer*0.1+0.5)
	Render(self.img,self.x,self.y,  PI/12,0.4-self.timer*0.01,self.timer*0.1+0.7)
	Render(self.img,self.x,self.y,PI/12*5,0.4-self.timer*0.01,self.timer*0.1+0.7)
	Render(self.img,self.x,self.y,PI/12*9,0.4-self.timer*0.01,self.timer*0.1+0.7)
end

function enemy_death_ef:frame()
	if self.timer==30 then Kill(self) end
end

Include'THlib\\enemy\\boss.lua'
