#pragma once
#include <iostream>

class SettingContainer
{
	private:
		/* Width of sidebar in pixels */
		static int sidebar_width;
		static float zoom_step;
		static float fps;
		static std::string gv_font_name;
		static int gv_font_size;
		static std::string tx_font_name;
		static int tx_font_size;
	public:
		int get_sidebar_width() {return sidebar_width;}
		void set_sidebar_width(int w) {sidebar_width = w;}
		float get_zoom_step() {return zoom_step;}
		void set_zoom_step(float z) {zoom_step = z;}
		float get_fps() {return fps;}
		void set_fps(float f) {fps = f;}
		 
		std::string get_gv_font_name() {return gv_font_name;}
		void set_gv_font_name(std::string n) {gv_font_name = n;}
		int get_gv_font_size() {return gv_font_size;}
		void set_gv_font_size(int s) {gv_font_size = s;}
		 
		std::string get_tx_font_name() {return tx_font_name;}
		void set_tx_font_name(std::string n) {tx_font_name = n;}
		int get_tx_font_size() {return tx_font_size;}
		void set_tx_font_size(int s) {tx_font_size = s;}
};
