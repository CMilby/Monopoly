//
//  player.cpp
//  Monopoly
//
//  Created by Craig Milby on 12/9/16.
//  Copyright © 2016 Craig Milby. All rights reserved.
//

#include "player.h"

Player::Player( int p_position ) : m_position( p_position ) {
	m_random = new Random( 1234 );
}

Player::Player( int p_position, int p_seed ) : m_position( p_position ) {
	m_random = new Random( p_seed );
}

int Player::Move( int p_diceRoll ) {
	m_position += p_diceRoll;
	m_position %= 40;
	
	// Go == 0
	// Jail == 10
	// Go To Jail == 30
	// Railroads == ( 5, 15, 25, 35 )
	// Utilites == ( 12, 28 )
	// Chance == ( 7, 22, 36 )
	// Community Chest == ( 2, 17, 33 )
	
	if ( m_position == 30 ) {
		return 1;
	} else if ( m_position == 7 || m_position == 22 || m_position == 36 ) {
		return 2;
	} else if ( m_position == 2 || m_position == 17 || m_position == 33 ) {
		return 3;
	}
	
	return 0;
}
