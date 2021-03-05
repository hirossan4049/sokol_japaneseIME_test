module main

import gg
import gx
import os
import math

const (
	win_width  = 1000
	win_height = 200
	bg_color   = gx.white
)

struct App {
mut:
	gg &gg.Context

    pending_text string
    text string
}

fn main() {
	mut app := &App{
		gg: 0
        text: ""
	}
	mut font_path := os.resource_abs_path(os.join_path('Cica-Regular.ttf'))

	app.gg = gg.new_context(
		width: win_width
		height: win_height
		use_ortho: true // This is needed for 2D drawing
		create_window: true
		window_title: 'japaneseIME'
		user_data: app
		bg_color: bg_color
		frame_fn: frame
		font_path: font_path // window_user_ptr: ctx

        ckj_input_pending: jip
        ckj_input_confirm: jic
	)
	app.gg.run()
}

fn jip(text string, length int, first int, caret int,mut app App){
    println("pending_text: $app.pending_text")
    app.pending_text = "$text"
}

fn jic(text string,mut app App){
    println("confirm text: $app.text")
    app.pending_text = ""
    app.text += "$text"
}

fn frame(mut app App) {
	app.gg.begin()
	width := gg.window_size().width
	mut scale_factor := math.round(f32(width) / win_width)
	if scale_factor <= 0 {
		scale_factor = 1
	}
	text_cfg := gx.TextCfg{
		size: 16 * int(scale_factor)
        color: gx.black
	}
	mut y := 10
	lines := app.text.split('\n')

    //draw_text := app.text + app.pending_textA
    tw := app.gg.text_width(app.text)
    app.gg.draw_text(10, y, app.text, text_cfg)

	text2_cfg := gx.TextCfg{
		size: 16 * int(scale_factor)
        color: gx.blue
	}
    app.gg.draw_text(tw+10, y, app.pending_text, text2_cfg)
	//for line in lines {
	//	app.gg.draw_text(10, y, line, text_cfg)
	//	y += 30
	//}
	app.gg.end()
}
