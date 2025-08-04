#ifndef __IPLAYERLISTENER_H__
#define __IPLAYERLISTENER_H__

class IPlayerListener
{
public:
	virtual void OnPlayerKilled(int lives_left) = 0;
	// CUSTOM
	virtual void OnPlayerPickedUpLife(int lives_left) = 0;
	// END 
};

#endif
