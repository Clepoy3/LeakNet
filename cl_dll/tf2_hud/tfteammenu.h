class ITF2TeamPanel
{
public:
	virtual void		Create( vgui::VPANEL parent ) = 0;
	virtual void		Destroy( void ) = 0;
};

extern ITF2TeamPanel* tf2teampanel;