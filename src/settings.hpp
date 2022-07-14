
class SettingContainer
{
	private:
		/* Width of sidebar in pixels */
		static int sidebar_width;
	public:
		void set_sidebar_width(int w) {sidebar_width = w;}
		int get_sidebar_width() {return sidebar_width;}
};
