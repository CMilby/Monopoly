//
//  player.h
//  Monopoly
//
//  Created by Craig Milby on 12/9/16.
//  Copyright © 2016 Craig Milby. All rights reserved.
//

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "random.h"

class Player {
	
private:
	int m_position;
	Random* m_random;
	
public:
	Player( int p_position );
	Player( int p_position, int p_seed );
	
	int Move( int p_diceRoll );
	
	inline void SetPosition( int p_position ) {
		if ( p_position < 0 ) {
			p_position += 40;
		}
		
		m_position = ( p_position % 40 );
	}
	
	inline int GetPosition() const {
		return m_position;
	}
	
	int Roll( bool &p_doubles ) {
		int myRoll1 = m_random->InRange( 1, 7 );
		int myRoll2 = m_random->InRange( 1, 7 );
		
		p_doubles = ( myRoll1 == myRoll2 );
		return ( myRoll1 + myRoll2 );
	}
};

#endif /* player_h */
