LoadPS('player_death_ef','THlib\\player\\player_death_ef.psi','parimg1')
LoadPS('graze','THlib\\player\\graze.psi','parimg6')

----Base class of all player characters (abstract)----
player=Class(object)

function player:init()
	self.group=GROUP_PLAYER
	self.y=-176
	self.supportx=0
	self.supporty=self.y
	self.hspeed=4
	self.lspeed=2
	self.collect_line=96
	self.slow=0
	self.layer=LAYER_PLAYER
	self.lr=1
	self.lh=0
	self.fire=0
	self.lock=false
	self.dialog=false
	self.nextshoot=0
	self.nextspell=0
	self.death=0
	self.protect=120
	lstg.player=self
	self.grazer=New(grazer)
	if not lstg.var.init_player_data then error('Player data has not been initialized. (Call function item.PlayerInit.)') end
	self.support=int(lstg.var.power/100)
	self.sp={}
end

function player:frame()
	--find target
	if ((not IsValid(self.target)) or (not self.target.colli)) then player.findtarget(self) end
	if not KeyIsDown'shoot' then self.target=nil end
	--lock
	if self.lock then return end
	--
	local dx=0
	local dy=0
	local v=self.hspeed
	if self.death==0 or self.death>90 then
		--input
		if KeyIsDown'slow' then self.slow=1 else self.slow=0 end
		if not self.dialog then
			if KeyIsDown'shoot' and self.nextshoot<=0 then self.class.shoot(self) PlaySound('plst00',0.15,self.x/200) end
			if KeyIsDown'spell' and self.nextspell<=0 and lstg.var.power>=100 and not lstg.var.block_spell then
				self.death=0
				item.PlayerSpell()
				lstg.var.power=lstg.var.power-100
				self.class.spell(self)
			end
		end
		--player move
		if self.slow==1 then v=self.lspeed end
		if KeyIsDown'up' then dy=dy+1 end
		if KeyIsDown'down' then dy=dy-1 end
		if KeyIsDown'left' then dx=dx-1 end
		if KeyIsDown'right' then dx=dx+1 end
		if dx*dy~=0 then v=v*SQRT2_2 end
		self.x=self.x+v*dx
		self.y=self.y+v*dy
		self.x=math.max(math.min(self.x,lstg.world.r-8),lstg.world.l+8)
		self.y=math.max(math.min(self.y,lstg.world.t-32),lstg.world.b+16)
		--item
		if self.y>self.collect_line then for i,o in ObjList(GROUP_ITEM) do o.attract=8 end
		else
			for i,o in ObjList(GROUP_ITEM) do
				local d=Dist(self,o)
				if (KeyIsDown'slow' and d<48) or d<24 then o.attract=max(o.attract,3) end
			end
		end
	elseif self.death==90 then
		item.PlayerMiss()
		New(bubble,'bubble1',self.x,self.y,20,1,6,Color(0xffffffff),Color(0x00ffffff),LAYER_PLAYER)
		New(player_death_ef,self.x,self.y)
		New(bullet_deleter,self.x,self.y)
	elseif self.death==84 then
		self.hide=true
		self.support=int(lstg.var.power/100)
	elseif self.death==50 then
		self.x=0
		self.supportx=0
		self.y=-236
		self.supporty=-236
		self.hide=false
	elseif self.death<50 then
		self.y=-176-1.2*self.death
	end
	--img
	if abs(self.lr)==1 then
		self.img=self.imgs[int(self.ani/8)%8+1]
	elseif self.lr==-6 then
		self.img=self.imgs[int(self.ani/8)%4+13]
	elseif self.lr== 6 then
		self.img=self.imgs[int(self.ani/8)%4+21]
	elseif self.lr<0 then
		self.img=self.imgs[7-self.lr]
	elseif self.lr>0 then
		self.img=self.imgs[15+self.lr]
	end
	--some status
	self.lr=self.lr+dx;
	if self.lr> 6 then self.lr= 6 end
	if self.lr<-6 then self.lr=-6 end
	if self.lr==0 then self.lr=self.lr+dx end
	if dx==0 then
		if self.lr> 1 then self.lr=self.lr-1 end
		if self.lr<-1 then self.lr=self.lr+1 end
	end

	self.lh=self.lh+(self.slow-0.5)*0.3
	if self.lh<0 then self.lh=0 end
	if self.lh>1 then self.lh=1 end

	if KeyIsDown'shoot' and not self.dialog then self.fire=self.fire+0.16 else self.fire=self.fire-0.16 end
	if self.fire<0 then self.fire=0 end
	if self.fire>1 then self.fire=1 end

	if self.nextshoot>0 then self.nextshoot=self.nextshoot-1 end
	if self.nextspell>0 then self.nextspell=self.nextspell-1 end

	if self.support>int(lstg.var.power/100) then self.support=self.support-0.0625
	elseif self.support<int(lstg.var.power/100) then self.support=self.support+0.0625 end
	if abs(self.support-int(lstg.var.power/100))<0.0625 then self.support=int(lstg.var.power/100) end

	self.supportx=self.x+(self.supportx-self.x)*0.6875
	self.supporty=self.y+(self.supporty-self.y)*0.6875

	if self.death>0 then self.death=self.death-1 end
	if self.protect>0 then self.protect=self.protect-1 end

	lstg.var.pointrate=item.PointRateFunc(lstg.var)
	--update supports
		if self.slist then
			self.sp={}
			if self.support==5 then
				for i=1,4 do self.sp[i]=MixTable(self.lh,self.slist[6][i]) self.sp[i][3]=1 end
			else
				local s=int(self.support)+1
				local t=self.support-int(self.support)
				for i=1,4 do
					if self.slist[s][i] and self.slist[s+1][i] then
						self.sp[i]=MixTable(t,MixTable(self.lh,self.slist[s][i]),MixTable(self.lh,self.slist[s+1][i]))
						self.sp[i][3]=1
					elseif self.slist[s+1][i] then
						self.sp[i]=MixTable(self.lh,self.slist[s+1][i])
						self.sp[i][3]=t
					end
				end
			end
		end
	--
end

function player:render()
	if self.protect%3==1 then SetImageState(self.img,'',Color(0xFF0000FF))
	else SetImageState(self.img,'',Color(0xFFFFFFFF)) end
	object.render(self)
end

function player:colli(other)
	if self.death==0 and self.protect==0 and not self.dialog then self.death=100 PlaySound('pldead00',0.5) end
end

function player:findtarget()
	self.target=nil
	local maxpri=-9999
	for i,o in ObjList(GROUP_ENEMY) do
		if o.colli then
			local dx=self.x-o.x
			local dy=self.y-o.y
			local pri=abs(dy)/(abs(dx)+0.01)
			if pri>maxpri then maxpri=pri self.target=o end
		end
	end
end

grazer=Class(object)

function grazer:init()
	self.layer=LAYER_ENEMY_BULLET_EF+50
	self.group=GROUP_PLAYER
	self.player=lstg.player
	self.list={}
	self.listpre={}
	self.grazed=false
	self.img='graze'
	ParticleStop(self)
	self.a=24
	self.b=24
end

function grazer:frame()
	self.x=self.player.x
	self.y=self.player.y
	self.hide=self.player.hide
	--
	for k,v in pairs(self.list) do
		if not self.listpre[k] then
			item.PlayerGraze()
		end
	end
	if self.grazed then
		PlaySound('graze',0.3,self.x/200)
		self.grazed=false
		ParticleFire(self)
	else ParticleStop(self) end
	self.listpre=self.list
	self.list={}
end

function grazer:render()
	object.render(self)
	SetImageState('player_aura','',Color(0xC0FFFFFF)*self.player.lh+Color(0x00FFFFFF)*(1-self.player.lh))
	Render('player_aura',self.x,self.y, self.timer*0.03,2-self.player.lh)
	SetImageState('player_aura','',Color(0xC0FFFFFF))
	Render('player_aura',self.x,self.y,-self.timer*0.03,self.player.lh)
end

function grazer:colli(other)
	if other.group~=GROUP_ENEMY then self.list[other[2]]=true end
end

player_bullet_straight=Class(object)

function player_bullet_straight:init(img,x,y,v,angle,dmg)
	self.group=GROUP_PLAYER_BULLET
	self.layer=LAYER_PLAYER_BULLET
	self.img=img
	self.x=x
	self.y=y
	self.rot=angle
	self.vx=v*cos(angle)
	self.vy=v*sin(angle)
	self.dmg=dmg
end

player_bullet_hide=Class(object)

function player_bullet_hide:init(a,b,x,y,v,angle,dmg,delay)
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
end

function player_bullet_hide:frame()
	if self.timer==self.delay then self.colli=true end
end

player_bullet_trail=Class(object)

function player_bullet_trail:init(img,x,y,v,angle,target,trail,dmg)
	self.group=GROUP_PLAYER_BULLET
	self.layer=LAYER_PLAYER_BULLET
	self.img=img
	self.x=x
	self.y=y
	self.rot=angle
	self.v=v
	self.target=target
	self.trail=trail
	self.dmg=dmg
end

function player_bullet_trail:frame()
	if IsValid(self.target) and self.target.colli then
		local a=math.mod(Angle(self,self.target)-self.rot+PI*4,PIx2)
		if a>PI then a=a-PIx2 end
		local da=self.trail/(Dist(self,self.target)+1)
		if da>=abs(a) then self.rot=Angle(self,self.target)
		else self.rot=self.rot+sign(a)*da end
	end
	self.vx=self.v*cos(self.rot)
	self.vy=self.v*sin(self.rot)
end

player_death_ef=Class(object)

function player_death_ef:init(x,y)
	self.x=x self.y=y self.img='player_death_ef' self.layer=LAYER_PLAYER+50
end

function player_death_ef:frame()
	if self.timer==4 then ParticleStop(self) end
	if self.timer==60 then Del(self) end
end

function MixTable(x,t1,t2)
	r={}
	local y=1-x
	if t2 then
		for i=1,#t1 do
			r[i]=y*t1[i]+x*t2[i]
		end
		return r
	else
		local n=int(#t1/2)
		for i=1,n do
			r[i]=y*t1[i]+x*t1[i+n]
		end
		return r
	end
end

Include'THlib\\player\\reimu\\reimu.lua'
Include'THlib\\player\\marisa\\marisa.lua'
