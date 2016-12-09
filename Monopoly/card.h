//
//  card.h
//  Monopoly
//
//  Created by Craig Milby on 12/9/16.
//  Copyright © 2016 Craig Milby. All rights reserved.
//

#ifndef __CARD_H__
#define __CARD_H__

#include <string>

class Card {
	
private:
	std::string m_text;
	int m_goto;
	
public:
	Card( const std::string &p_text, int p_goto ) : m_text( p_text ), m_goto( p_goto ) {
		
	}
	
	inline int Goto() const {
		return m_goto;
	}
};

#endif /* card_h */
