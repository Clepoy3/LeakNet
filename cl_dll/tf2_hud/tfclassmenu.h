// IMyPanel.h
class ITF2ClassPanel
{
public:
	virtual void		Create( vgui::VPANEL parent ) = 0;
	virtual void		Destroy( void ) = 0;
};

extern ITF2ClassPanel* tf2classpanel;