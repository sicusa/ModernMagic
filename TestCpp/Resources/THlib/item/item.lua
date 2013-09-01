LoadTexture('item','THlib\\item\\item.png')
LoadImageGroup('item','item',0,0,32,32,2,4,8,8)
LoadImageGroup('item_up','item',64,0,32,32,2,4)
SetImageState('item8','',Color(0xC0FFFFFF))

item=Class(object)

function item:init(x,y,t,v,angle)
	x=min(max(x,-184),184)
	self.x=x
	self.y=y
	angle=angle or PI_2
	v=v or 1.5
	SetV(self,v,angle)
	self.v=v
	self.group=GROUP_ITEM
	self.layer=LAYER_ITEM
	self.bound=false
	self.img='item'..t
	self.imgup='item_up'..t
	self.attract=0
end

function item:render()
	if self.y>224 then Render(self.imgup,self.x,216) else object.render(self) end
end

function item:frame()
	if self.timer<24 then
		self.rot=self.rot+PI_4
		self.hscale=(self.timer+25)/48
		self.vscale=self.hscale
		if self.timer==22 then self.vy=min(self.v,2) self.vx=0 end
	elseif self.attract>0 then
		local a=Angle(self,lstg.player)
		self.vx=self.attract*cos(a)+lstg.player.dx*0.5
		self.vy=self.attract*sin(a)+lstg.player.dy*0.5
	else self.vy=max(self.dy-0.03,-1.7) end
	if self.y<-256 then Del(self) end
end

function item:colli(other)
	if other==lstg.player then
		if self.class.collect then self.class.collect(self) end
		Kill(self)
		PlaySound('item00',0.3,self.x/200)
	end
end

function GetPower(v)
	local before=int(lstg.var.power/100)
	lstg.var.power=min(500,lstg.var.power+v)
	local after=int(lstg.var.power/100)
	if after>before then PlaySound('powerup',0.3) end
	if lstg.var.power==500 then
		for i,o in ObjList(GROUP_ITEM) do
			if o.class==item_power or o.class==item_power_large then
				o.class=item_faith
				o.img='item5'
				o.imgup='item_up5'
				New(bubble,'parimg12',o.x,o.y,16,0.5,1,Color(0xFF00FF00),Color(0x0000FF00),LAYER_ITEM+50)
			end
		end
	end
end

item_power=Class(item)
function item_power:init(x,y,v,a) item.init(self,x,y,1,v,a) end
function item_power:collect() GetPower(5) end

item_power_large=Class(item)
function item_power_large:init(x,y,v,a) item.init(self,x,y,6,v,a) end
function item_power_large:collect() GetPower(100)  end

item_power_full=Class(item)
function item_power_full:init(x,y) item.init(self,x,y,4) end
function item_power_full:collect() GetPower(500)  end

item_extend=Class(item)
function item_extend:init(x,y) item.init(self,x,y,7) end
function item_extend:collect()
	lstg.var.lifeleft=lstg.var.lifeleft+1
	PlaySound('extend',0.5)
	New(hinter,'hint.extend',0.6,0,112,15,120)
end

item_chip=Class(item)
function item_chip:init(x,y) item.init(self,x,y,3) end
function item_chip:collect()
	lstg.var.chip=lstg.var.chip+1
	if lstg.var.chip==5 then
		lstg.var.lifeleft=lstg.var.lifeleft+1
		lstg.var.chip=0
		PlaySound('extend',0.5)
		New(hinter,'hint.extend',0.6,0,112,15,120)
	end
end

item_faith=Class(item)
function item_faith:init(x,y) item.init(self,x,y,5) end
function item_faith:collect()
	local var=lstg.var
	New(float_text,'item','10000',self.x,self.y+6,0.75,Angle(lstg.player,self),60,0.5,0.5,Color(0x8000C000),Color(0x0000C000))
	var.faith=var.faith+100
	var.score=var.score+10000
end

item_faith_minor=Class(item)
function item_faith_minor:init(x,y) item.init(self,x,y,8) self.vx=0 self.vy=0 end
function item_faith_minor:frame()
	if self.timer>=24 then
		SetV(self,8,Angle(self,lstg.player))
	end
end
function item_faith_minor:collect()
	local var=lstg.var
	var.faith=var.faith+5
	var.score=var.score+500
end

item_point=Class(item)
function item_point:init(x,y) item.init(self,x,y,2) end
function item_point:collect()
	local var=lstg.var
	if lstg.player.y>lstg.player.collect_line then
		New(float_text,'item',var.pointrate,self.x,self.y+6,0.75,Angle(lstg.player,self),60,0.5,0.5,Color(0x80FFFF00),Color(0x00FFFF00))
		var.score=var.score+var.pointrate
	else
		New(float_text,'item',var.pointrate/2,self.x,self.y+6,0.75,Angle(lstg.player,self),60,0.5,0.5,Color(0x80FFFFFF),Color(0x00FFFFFF))
		var.score=var.score+var.pointrate/2
	end
end

function item.DropItem(x,y,drop)
	local n=drop[1]+drop[2]+drop[3]
	local r=sqrt(n-1)*5
	if lstg.var.power==500 then drop[2]=drop[2]+drop[1] drop[1]=0 end
	for i=1,drop[1] do
		local r2=sqrt(ran:Float(1,4))*r
		local a=ran:Float(0,PIx2)
		New(item_power,x+r2*cos(a),y+r2*sin(a))
	end
	for i=1,drop[2] do
		local r2=sqrt(ran:Float(1,4))*r
		local a=ran:Float(0,PIx2)
		New(item_faith,x+r2*cos(a),y+r2*sin(a))
	end
	for i=1,drop[3] do
		local r2=sqrt(ran:Float(1,4))*r
		local a=ran:Float(0,PIx2)
		New(item_point,x+r2*cos(a),y+r2*sin(a))
	end
end

item.sc_bonus_max=2000000
item.sc_bonus_base=1000000

function item.StartChipBonus()
	lstg.var.chip_bonus=true
end

function item.EndChipBonus(x,y)
	if lstg.var.chip_bonus then New(item_chip,x,y) lstg.var.chip_bonus=false end
end

function item.PlayerInit()
	lstg.var.power=0
	lstg.var.lifeleft=2
	lstg.var.chip=0
	lstg.var.faith=50000
	lstg.var.graze=0
	lstg.var.score=0
	lstg.var.pointrate=item.PointRateFunc(lstg.var)
	lstg.var.block_spell=false
	lstg.var.chip_bonus=false
	lstg.var.init_player_data=true
end

function item.PlayerMiss()
	lstg.var.chip_bonus=false
	if lstg.var.sc_bonus then lstg.var.sc_bonus=0 end
	lstg.player.protect=360
	lstg.var.lifeleft=lstg.var.lifeleft-1
	lstg.var.power=math.max(lstg.var.power-320,0)
	if lstg.var.lifeleft>0 then
		for i=1,7 do
			local a=PI_2+(i-4)*0.3+lstg.player.x*0.0047
			if i%2==1 then New(item_power,lstg.player.x,lstg.player.y+10,3,a)
			else New(item_power_large,lstg.player.x,lstg.player.y+10,3,a) end
		end
	else New(item_power_full,lstg.player.x,lstg.player.y+10) end
end

function item.PlayerSpell()
	if lstg.var.sc_bonus then lstg.var.sc_bonus=0 end
end

function item.PlayerGraze()
	lstg.var.graze=lstg.var.graze+1
	lstg.var.score=lstg.var.score+50
	lstg.player.grazer.grazed=true
end

function item.PointRateFunc(var)
	local r=var.faith*(1+var.graze*0.0001)
	return r-r%20
end

function item.DrawScore()
	Render('hint.hiscore',432,436,0,0.5)
	Render('hint.score',432,412,0,0.5)
	Render('hint.player',432,388,0,0.5)
	Render('hint.graze',432,364,0,0.5)
	RenderText('number',string.format('%09d\n%09d\n%d(%d/5)\n%d',max(lstg.tmpvar.hiscore or 0,lstg.var.score),lstg.var.score,max(0,lstg.var.lifeleft),lstg.var.chip,lstg.var.graze),628,436,0.5,'right')
	Render('hint.power',432,308,0,0.5)
	Render('hint.faith',432,290,0,0.5)
	Render('hint.point',432,272,0,0.5)
	RenderText('number',string.format('%.2f/5.00\n%d\n%d',lstg.var.power/100,lstg.var.faith,lstg.var.pointrate),628,320,0.375,'right')
end