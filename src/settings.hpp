
class SettingContainer
{
	private:
		/* Width of sidebar in pixels */
		static int sidebar_width;
		static float zoom_step;
	public:
		void set_sidebar_width(int w) {sidebar_width = w;}
		int get_sidebar_width() {return sidebar_width;}
		void set_zoom_step(float z) {zoom_step = z;}
		float get_zoom_step() {return zoom_step;}
};
