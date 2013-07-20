local bits={128,64,32,16,8,4,2,1}
local keyname={'up','down','left','right','slow','shoot','spell','special'}
function KeyState2Bin()
	local result=0
	for i,k in pairs(keyname) do
		if KeyState[k] then result=result+bits[i] end
	end
	return result
end

function Bin2KeyState(bin)
	for i,k in pairs(keyname) do
		if bin>=bits[i] then KeyState[k]=true bin=bin-bits[i] else KeyState[k]=false end
	end
end

ext={replay={}}

ext.replay.infolen=2048
ext.replay.sts={}
ext.replay.sts._mode='none'
ext.replay.sts._filename='none'

ext.pause_menu_text={{'Return to Game','Return to Title','Give up and Retry'},{'Return to Game','Return to Title','Replay Again'}}

function ext.replay.IsReplay()
	return ext.replay.sts.mode=='load'
end

function stage.Set(mode,rep_name,stage_name)
	ext.pause_menu_order=nil
	if mode=='save' then
		lstg.var.stage_name=stage_name
		lstg.var.ran_seed=((os.time()%65536)*877)%65536
		ran:Seed(lstg.var.ran_seed)
		ext.replay.sts._mode='save'
		ext.replay.sts._filename=rep_name
		stage.next_stage=stage.stages[lstg.var.stage_name]
	elseif mode=='load' then
		ext.replay.sts._mode='load'
		ext.replay.sts._filename=rep_name
		stage.next_stage=true
	else
		lstg.var.stage_name=stage_name
		ext.replay.sts._mode='none'
		ext.replay.sts._filename='none'
		stage.next_stage=stage.stages[lstg.var.stage_name]
	end
	if (not stage.current_stage.is_menu) and (not ext.replay.IsReplay()) then
		str=stage.current_stage.stage_name..'@'..lstg.var.player_name
		if scoredata.hiscore[str]==nil then scoredata.hiscore[str]=0 end
		scoredata.hiscore[str]=max(scoredata.hiscore[str],lstg.var.score)
	end
end

function stage.Restart()
	stage.preserve_res=true
	stage.Set(ext.replay.sts._mode,ext.replay.sts._filename,lstg.var.stage_name)
end

function GetInput()
	if stage.next_stage then
		lstg.world.l=-192
		lstg.world.r= 192
		lstg.world.b=-224
		lstg.world.t= 224
		lstg.tmpvar={}
		if ext.replay.sts.file then
			if ext.replay.sts.mode=='save' then
				local s=Serialize(lstg.var)
				ext.replay.sts.file:write(s..string.rep(' ',ext.replay.infolen-#s))
			end
			ext.replay.sts.file:close()
			ext.replay.sts.file=nil
		end
		if ext.replay.sts.zipfile then ext.replay.sts.zipfile:close() ext.replay.sts.zipfile=nil end
		ext.replay.sts.mode=ext.replay.sts._mode
		ext.replay.sts.filename=ext.replay.sts._filename
		if ext.replay.sts.mode=='save' then
			ext.replay.sts.file=assert(io.open('replay\\'..ext.replay.sts.filename,'wb'))
			local s=Serialize(lstg.var)
			ext.replay.sts.file:write(s..string.rep(' ',ext.replay.infolen-#s))
		elseif ext.replay.sts.mode=='load' then
			if type(ext.replay.sts.filename)=='string' then
				ext.replay.sts.file=assert(io.open('replay\\'..ext.replay.sts.filename,'rb'))
			else
				ext.replay.sts.zipfile=assert(zip.open('replay\\'..ext.replay.sts.filename[1]))
				ext.replay.sts.file=assert(ext.replay.sts.zipfile:open('replay/'..ext.replay.sts.filename[2]))
			end
			lstg.var=DeSerialize(ext.replay.sts.file:read(ext.replay.infolen))
			stage.next_stage=stage.stages[lstg.var.stage_name]
		else
			--nothing
		end
		if lstg.var.ran_seed then ran:Seed(lstg.var.ran_seed) end
		KeyStatePre={}
		if scoredata.hiscore==nil then scoredata.hiscore={} end
		if not stage.next_stage.is_menu then
			lstg.tmpvar.hiscore=scoredata.hiscore[stage.next_stage.stage_name..'@'..tostring(lstg.var.player_name)]
		end
	else
		for k,v in pairs(setting.keys) do
			KeyStatePre[k]=KeyState[k]
		end
	end
	if ext.replay.sts.mode=='save' then
		for k,v in pairs(setting.keys) do
			KeyState[k]=KeyIsDown_(v)
		end
		ext.replay.sts.file:write(string.char(KeyState2Bin()))
	elseif ext.replay.sts.mode=='load' then
		local char=ext.replay.sts.file:read(1)
		if char then Bin2KeyState(string.byte(char))
		else error('Unexpected end of replay file.') end
	else
		for k,v in pairs(setting.keys) do
			KeyState[k]=KeyIsDown_(v)
		end
	end
end

ext.replay.ticker=0

function FrameFunc()
	if GetLastKey()==KEY_HOME then Snapshot('snapshot\\'..os.date("!%Y-%m-%d-%H-%M-%S",os.time()+setting.timezone*3600)..'.bmp') end
	if ext.pause_menu==nil then
		if ext.replay.IsReplay() then
			ext.replay.ticker=ext.replay.ticker+1
			if KeyIsDown_(KEY_CTRL) then
				DoFrame(true,false)
				ext.pause_menu_order=nil
				DoFrame(true,false)
				ext.pause_menu_order=nil
				DoFrame(true,false)
				ext.pause_menu_order=nil
				DoFrame(true,true)
				ext.pause_menu_order=nil
			elseif KeyIsDown_(KEY_SHIFT) then
				if ext.replay.ticker%4==0 then
					DoFrame(true,true)
					ext.pause_menu_order=nil
				else
					DoFrame(false,true)
				end
			else
				DoFrame(true,true)
				ext.pause_menu_order=nil
			end
		else
			DoFrame(true,true)
		end
		if (GetLastKey()==KEY_ESCAPE or ext.pop_pause_menu) and not stage.current_stage.is_menu then
			ext.pop_pause_menu=nil
			PlaySound('pause',0.5)
			local _,bgm=EnumRes('bgm')
			for _,v in pairs(bgm) do
				if GetMusicState(v)~='stopped' then PauseMusic(v) end
			end
			ext.pause_menu={}
			ext.pause_menu.pos=1
			task.New(ext.pause_menu,function()
				ext.pause_menu.lock=true
				for i=1,30 do
					ext.mask_color=Color(i*7,0,0,0)
					ext.font_alpha=i*8.5
					task.Wait(1)
				end
				ext.pause_menu.lock=nil
			end)
		end
	else
		local m
		if ext.replay.IsReplay() then m=2 else m=1 end
		if GetLastKey()==setting.keys.up then ext.pause_menu.pos=ext.pause_menu.pos-1 PlaySound('select00',0.3) end
		if GetLastKey()==setting.keys.down then ext.pause_menu.pos=ext.pause_menu.pos+1 PlaySound('select00',0.3) end
		ext.pause_menu.pos=(ext.pause_menu.pos-1)%(#(ext.pause_menu_text[m]))+1
		--
		task.Do(ext.pause_menu)
		DoFrame(false,true)
		if (GetLastKey()==KEY_ESCAPE or GetLastKey()==setting.keys.shoot) and ext.pause_menu and not ext.pause_menu.lock then
			if GetLastKey()==setting.keys.shoot then
				PlaySound('ok00',0.3)
				ext.pause_menu_order=ext.pause_menu_text[m][ext.pause_menu.pos]
			end
			task.New(ext.pause_menu,function()
				ext.pause_menu.lock=true
				for i=30,1,-1 do
					ext.mask_color=Color(i*7,0,0,0)
					ext.font_alpha=i*8.5
					task.Wait(1)
				end
				task.New(stage.current_stage,function()
					task.Wait(1)
					local _,bgm=EnumRes('bgm')
					for _,v in pairs(bgm) do
						if GetMusicState(v)~='stopped' then ResumeMusic(v) end
					end
				end)
				ext.pause_menu=nil
			end)
		end
	end
	if lstg.quit_flag then GameExit() end
	return lstg.quit_flag
end

function AfterRender()
	if ext.pause_menu then
		SetViewMode'ui'
		SetImageState('white','',ext.mask_color)
		RenderRect('white',0,640,0,480)
		--
		local m
		if ext.replay.IsReplay() then m=2 else m=1 end
		--
		local pause_menu_text
		if lstg.tmpvar.pause_menu_text then
			pause_menu_text=lstg.tmpvar.pause_menu_text
		else
			pause_menu_text=ext.pause_menu_text[m]
		end
		--
		local w=0
		for i=1,#pause_menu_text do w=max(w,#pause_menu_text[i]) end
		w=w+2
		SetImageState('white','',Color(ext.font_alpha,128,128,128))
		RenderRect('white',320-w*6,320+w*6,240+#pause_menu_text*11.5-ext.pause_menu.pos*23,240+#pause_menu_text*11.5-ext.pause_menu.pos*23+23)
		local text=table.concat(pause_menu_text,'\n')
		SetFontState('menu','',Color(ext.font_alpha,0,0,0))
		RenderText('menu',text,321,239,0.5,'centerpoint')
		SetFontState('menu','',Color(ext.font_alpha,255,255,255))
		RenderText('menu',text,320,240,0.5,'centerpoint')
	end
end

function ext.GetPauseMenuOrder()
	return ext.pause_menu_order
end

function FocusLoseFunc()
	if ext.pause_menu==nil and not stage.current_stage.is_menu then  ext.pop_pause_menu=true end
end

stage.group={}

function stage.group.New(title,stages)
	sg={['title']=title,number=#stages}
	for i=1,#stages do
		sg[i]=stages[i]
		local s=stage.New(stages[i])
		s.frame=stage.group.frame
		s.render=stage.group.render
		s.number=i
		s.group=sg
		sg[stages[i]]=s
	end
	return sg
end

function stage.group.frame(self)
	if not lstg.var.init_player_data then error('Player data has not been initialized. (Call function item.PlayerInit.)') end
	if lstg.var.lifeleft<=-1 then
		stage.group.ReturnToTitle'all'
	end
	if ext.GetPauseMenuOrder()=='Return to Title' then
		stage.group.ReturnToTitle'none'
	end
	if ext.GetPauseMenuOrder()=='Replay Again' then stage.Restart() end
	if ext.GetPauseMenuOrder()=='Give up and Retry' then stage.group.Start(self.group) end
end

function stage.group.render(self)
	SetViewMode'ui'
	misc.DrawFrame()
	if lstg.var.init_player_data then item.DrawScore() end
end

function stage.group.frame_sc_pr(self)
	if not lstg.var.init_player_data then error('Player data has not been initialized. (Call function item.PlayerInit.)') end
	if lstg.var.lifeleft<=-1 then
		stage.Restart()
	end
	if ext.GetPauseMenuOrder()=='Return to Title' then
		stage.group.ReturnToTitle'none'
	end
	if ext.GetPauseMenuOrder()=='Replay Again' then stage.Restart() end
	if ext.GetPauseMenuOrder()=='Give up and Retry' then stage.Restart() end
end

function stage.group.Start(group)
	stage.Set('save','temp\\'..group[1],group[1])
end

function stage.group.FinishStage()
	local self=stage.current_stage
	if self.number==self.group.number then
		stage.group.ReturnToTitle'all'
	else
		if ext.replay.IsReplay() then
			stage.Set('load',{ext.replay.sts.filename[1],self.group[self.number+1]})
		else
			stage.Set('save',self.group[self.number+1],self.group[self.number+1])
		end
	end
end

function stage.group.ReturnToTitle(rep)
	local self=stage.current_stage
	local title=stage.stages[self.group.title]
	if ext.replay.IsReplay() then title.save_replay=nil else
		if rep=='all' then
			title.save_replay={}
			for i=1,self.number do
				title.save_replay[i]=self.group[i]
			end
		elseif rep=='single' then
			title.save_replay={self.group[self.number]}
		else title.save_replay=nil end
	end
	stage.Set('none','none',self.group.title)
end