//
//  tile.h
//  Monopoly
//
//  Created by Craig Milby on 12/9/16.
//  Copyright © 2016 Craig Milby. All rights reserved.
//

#ifndef __TILE_H__
#define __TILE_H__

#include <string>

class Tile {
	
private:
	std::string m_tileName;
	int m_cost;
	
public:
	Tile ( const std::string &p_name, int p_cost );

	std::string GetName() const {
		return m_tileName;
	}
};

#endif /* tile_h */
