LoadTexture('laser','THlib\\laser\\laser.png')
LoadImageGroup('laser_1','laser',  0,0, 64,32,1,8)
LoadImageGroup('laser_2','laser', 64,0,128,32,1,8)
LoadImageGroup('laser_3','laser',192,0, 64,32,1,8)

for i=1,3 do
	for j=1,8 do
		SetImageCenter('laser_'..i..j,0,16)
	end
end

LoadImageGroup('laser_node','bullet1',80,0,32,32,1,8)

laser=Class(object)

function laser:init(index,x,y,rot,l1,l2,l3,d)
	self.index=index self.x=x self.y=y self.rot=rot
	self.l1=l1 self.l2=l2 self.l3=l3 self.d0=d self.d=0
	self.alpha=0
	self.node=1
	self.group=GROUP_INDES
	self.layer=LAYER_ENEMY_BULLET
	self.colli=false
	self.dd=0
	self.da=0
	self.counter=0
end

function laser:frame()
	task.Do(self)
	if self.counter>0 then
		self.counter=self.counter-1
		self.d=self.d+self.dd
		self.alpha=self.alpha+self.da
	end
	if self.alpha<0.999 then return end
	--collide
	local x=lstg.player.x-self.x
	local y=lstg.player.y-self.y
	local rot=self.rot
	x,y=x*cos(rot)+y*sin(rot),y*cos(rot)-x*sin(rot)
	y=abs(y)
	if x>0 then
		if x<self.l1 then
			if y<x/self.l1*self.d/2 then player.colli(lstg.player,self) end
		elseif x<self.l1+self.l2 then
			if y<self.d/2 then player.colli(lstg.player,self) end
		elseif x<self.l1+self.l2+self.l3 then
			if y<(self.l1+self.l2+self.l3-x)/self.l3*self.d/2 then player.colli(lstg.player,self) end
		end
		if self.timer%6==0 then
			if x<self.l1 then
				if y<x/self.l1*self.d/2+16 then item.PlayerGraze() end
			elseif x<self.l1+self.l2 then
				if y<self.d/2+16 then item.PlayerGraze() end
			elseif x<self.l1+self.l2+self.l3 then
				if y<(self.l1+self.l2+self.l3-x)/self.l3*self.d/2+16 then item.PlayerGraze() end
			end
		end
	end
end

function laser:render()
	if self.d==0 then return end
	local c=Color(255*self.alpha,255,255,255)
	SetImageState('laser_1'..self.index,'mul+add',c)
	Render('laser_1'..self.index,self.x,self.y,self.rot,self.l1/64,self.d/16)
	SetImageState('laser_2'..self.index,'mul+add',c)
	Render('laser_2'..self.index,self.x+self.l1*cos(self.rot),self.y+self.l1*sin(self.rot),self.rot,self.l2/128,self.d/16)
	SetImageState('laser_3'..self.index,'mul+add',c)
	Render('laser_3'..self.index,self.x+(self.l1+self.l2)*cos(self.rot),self.y+(self.l1+self.l2)*sin(self.rot),self.rot,self.l3/64,self.d/16)
	c=Color(255*self.d/self.d0,255,255,255)
	SetImageState('laser_node'..self.index,'mul+add',c)
	Render('laser_node'..self.index,self.x,self.y, self.timer*0.02,self.node*self.d0/12)
	Render('laser_node'..self.index,self.x,self.y,-self.timer*0.02,self.node*self.d0/12)
end

function laser:del()
	if not self.off then
		PreserveObject(self)
	end
	if not self.dead then
		self.dead=true
		task.New(self,function()
			laser.TurnOff(self,30)
			task.Wait(30)
			self.off=true
			RawDel(self)
		end)
	end
end

function laser:kill()
	if not self.off then
		PreserveObject(self)
	end
	if not self.dead then
		for l=0,self.l1+self.l2+self.l3,12 do New(item_faith_minor,self.x+l*cos(self.rot),self.y+l*sin(self.rot)) end
		self.dead=true
		task.New(self,function()
			laser.TurnOff(self,30)
			task.Wait(30)
			self.off=true
			RawKill(self)
		end)
	end
end

function laser:TurnOn(t)
	t=t or 30
	PlaySound('lazer00',0.25,self.x/200)
	self.counter=t
	self.da=(1-self.alpha)/t
	self.dd=(self.d0-self.d)/t
end
function laser:TurnHalfOn(t)
	t=t or 30
	self.counter=t
	self.da=(0.5-self.alpha)/t
	self.dd=(0.5*self.d0-self.d)/t
end
function laser:TurnOff(t)
	t=t or 30
	self.counter=t
	self.da=-self.alpha/t
	self.dd=-self.d/t
end