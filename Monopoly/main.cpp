//
//  main.cpp
//  Monopoly
//
//  Created by Craig Milby on 12/9/16.
//  Copyright © 2016 Craig Milby. All rights reserved.
//

#include <iostream>
#include <map>
#include <vector>

#include <limits.h>

#include "card.h"
#include "player.h"
#include "random.h"
#include "tile.h"

struct PairCompare {
	inline bool operator() ( const std::pair<std::string, unsigned long>& p_pair1, const std::pair<std::string, unsigned long>& p_pair2 ) {
		return p_pair1.second < p_pair2.second;
	}
};

void PopulateTiles( std::map<int, Tile*> &p_tilePos, std::map<Tile*, unsigned long> &p_tileLands );
void PopulateCommunityChest( std::vector<Card*> &p_cards, Random* p_rand );
void PopulateChance( std::vector<Card*> &p_cards, Random* p_rand );

int main ( int argc, const char * argv[] ) {
	Random* rand = new Random( ( unsigned int ) time( 0 ) );
	
	std::map<int, Tile*> tilePos;
	std::map<Tile*, unsigned long> tileLands;
	PopulateTiles( tilePos, tileLands );
	
	std::vector<Card*> communityChests;
	std::vector<Card*> chances;
	PopulateCommunityChest( communityChests, rand );
	PopulateChance( chances, rand );
	
	// Player* player0 = new Player( 0, 12345 );
	unsigned int myNumPlayers = 4;
	std::vector<Player*> players;
	for ( unsigned int i = 0; i < myNumPlayers; i++ ) {
		players.push_back( new Player( 0, rand->InRange( 0, INT_MAX ) ) );
	}
	
	unsigned int myGamesToPlay = 1000;
	for ( unsigned int i = 0; i < myGamesToPlay; i++ ) {
		int myTurn = 0;
		int myMaxTurns = 1000;
		while ( myTurn < myMaxTurns ) {
			myTurn++;
			
			for ( unsigned int i = 0; i < myNumPlayers; i++ ) {
				bool isDouble = false;
				int myNumDoubles = 0;
				do {
					int myRoll = players[ i ]->Roll( isDouble );
					if ( isDouble ) {
						myNumDoubles++;
						isDouble = true;
					} else {
						isDouble = false;
					}
					
					int myMove = -1;
					if ( myNumDoubles == 3 ) {
						players[ i ]->SetPosition( 10 );
					} else {
						myMove = players[ i ]->Move( myRoll );
					}
					
					tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
					
					if ( myMove == 1 ) {
						players[ i ]->SetPosition( 10 );
						tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
					} else if ( myMove == 3 ) {
						// Community Chest
						Card* myCard = chances[ 0 ];
						chances.erase( chances.begin() );
						chances.push_back( myCard );
						
						if ( myCard->Goto() >= 0 ) {
							players[ i ]->SetPosition( myCard->Goto() );
							tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
						}
					} else if ( myMove == 2 ) {
						// Chance
						Card* myCard = chances[ 0 ];
						chances.erase( chances.begin() );
						chances.push_back( myCard );
						
						if ( myCard->Goto() >= 0 ) {
							players[ i ]->SetPosition( myCard->Goto() );
							tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
						} else if ( myCard->Goto() == -2 ) {
							// Nearest Utility
							int myPos = players[ i ]->GetPosition();
							int ecDiff = 11 - myPos;
							if ( ecDiff <= 0 ) {
								ecDiff += 40;
							}
							
							int wwDiff = 28 - myPos;
							if ( wwDiff <= 0 ) {
								wwDiff += 40;
							}
							
							if ( wwDiff << ecDiff ) {
								players[ i ]->SetPosition( 28 );
							} else {
								players[ i ]->SetPosition( 11 );
							}
							
							tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
						} else if ( myCard->Goto() == -3 ) {
							// Nearest Railroad
							int myPos = players[ i ]->GetPosition();
							int rrDiff = 5 - myPos;
							if ( rrDiff <= 0 ) {
								rrDiff += 40;
							}
							
							int paDiff = 15 - myPos;
							if ( paDiff <= 0  ) {
								paDiff += 40;
							}
							
							int boDiff = 25 - myPos;
							if ( boDiff <= 0 ) {
								boDiff += 40;
							}
							
							int slDiff = 35 - myPos;
							if ( slDiff <= 0 ) {
								slDiff += 40;
							}
							
							if ( slDiff < boDiff && slDiff < paDiff && slDiff < rrDiff ) {
								players[ i ]->SetPosition( 35 );
							} else if ( boDiff < slDiff && boDiff < paDiff && boDiff < rrDiff ) {
								players[ i ]->SetPosition( 25 );
							} else if ( paDiff < slDiff && paDiff < boDiff && paDiff < rrDiff ) {
								players[ i ]->SetPosition( 15 );
							} else if ( rrDiff < slDiff && rrDiff < boDiff && rrDiff < paDiff ) {
								players[ i ]->SetPosition( 5 );
							}
							
							tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
						} else if ( myCard->Goto() == -4 ) {
							// Back 3 spaces
							players[ i ]->SetPosition( players[ i ]->GetPosition() - 3 );
							tileLands[ tilePos[ players[ i ]->GetPosition() ] ]++;
						}
					}
				} while ( isDouble );
			}
		}
	}
	
	std::vector<std::pair<std::string, unsigned long>> results;
	float myTotalMoves = 0;
	
	for ( std::map<Tile*, unsigned long>::iterator it = tileLands.begin(); it != tileLands.end(); it++ ) {
		// std::cout << it->first->GetName() << ": " << it->second << std::endl;
		results.push_back( std::pair<std::string, unsigned long>( it->first->GetName(), it->second ) );
		myTotalMoves += ( float ) it->second;
	}

	
	std::sort( results.begin(), results.end(), PairCompare() );
	for ( unsigned int i = 0; i < results.size(); i++ ) {
		std::cout << results[ i ].first << ": " << ( ( float ) results[ i ].second / myTotalMoves ) * 100.0f << std::endl;
	}
	
	return 0;
}

void PopulateCommunityChest( std::vector<Card*> &p_cards, Random* p_rand ) {
	p_cards.push_back( new Card( "Advance to Go", 0 ) );
	p_cards.push_back( new Card( "Bank Error", -1 ) );
	p_cards.push_back( new Card( "Doctors Fee", -1 ) );
	p_cards.push_back( new Card( "Stock Sale", -1 ) );
	p_cards.push_back( new Card( "Get Outta Jail", -1 ) );
	p_cards.push_back( new Card( "Go To Jail", 10 ) );
	p_cards.push_back( new Card( "Grand Opera", -1 ) );
	p_cards.push_back( new Card( "Holiday Fund", -1 ) );
	p_cards.push_back( new Card( "Income Tax", -1 ) );
	p_cards.push_back( new Card( "Birthday", -1 ) );
	p_cards.push_back( new Card( "Life Insurance", -1 ) );
	p_cards.push_back( new Card( "Pay Hospital", -1 ) );
	p_cards.push_back( new Card( "Pay School Fee", -1 ) );
	p_cards.push_back( new Card( "Receive $25", -1 ) );
	p_cards.push_back( new Card( "Street Repairs", -1 ) );
	p_cards.push_back( new Card( "Won Second Prize", -1 ) );
	p_cards.push_back( new Card( "You Inherit", -1 ) );
	
	for ( int i = 0; i < p_cards.size(); i++ ) {
		int r = i + p_rand->InRange( 0, INT_MAX ) % ( p_cards.size() - i );
		Card* temp = p_cards[ i ];
		p_cards[ i ] = p_cards[ r ];
		p_cards[ r ] = temp;
	}
}

void PopulateChance( std::vector<Card*> &p_cards, Random* p_rand ) {
	p_cards.push_back( new Card( "Advance to Go", 0 ) );
	p_cards.push_back( new Card( "Advance to Illinois Ave.", 24 ) );
	p_cards.push_back( new Card( "Advance to St. Charles Place", 11 ) );
	p_cards.push_back( new Card( "Advance to nearest utility", -2 ) );
	p_cards.push_back( new Card( "Advance to nearest railroad", -3 ) );
	p_cards.push_back( new Card( "Bank pays you", -1 ) );
	p_cards.push_back( new Card( "Get outta jail", -1 ) );
	p_cards.push_back( new Card( "Go back 3 spaces", -4 ) );
	p_cards.push_back( new Card( "Go to jail", 10 ) );
	p_cards.push_back( new Card( "Make repairs", -1 ) );
	p_cards.push_back( new Card( "Poor tax", -1 ) );
	p_cards.push_back( new Card( "Take a trip on the Reading", 5 ) );
	p_cards.push_back( new Card( "Take a walk on the boardwalk", 39 ) );
	p_cards.push_back( new Card( "Pay each player", -1 ) );
	p_cards.push_back( new Card( "Building loan", -1 ) );
	p_cards.push_back( new Card( "You won a competition", -1 ) );
	
	for ( int i = 0; i < p_cards.size(); i++ ) {
		int r = i + p_rand->InRange( 0, INT_MAX ) % ( p_cards.size() - i );
		Card* temp = p_cards[ i ];
		p_cards[ i ] = p_cards[ r ];
		p_cards[ r ] = temp;
	}
}

void PopulateTiles( std::map<int, Tile*> &p_tilePos, std::map<Tile*, unsigned long> &p_tileLands ) {
	// Go == 0
	// Jail == 10
	// Go To Jail == 30
	// Railroads == ( 5, 15, 25, 35 )
	// Utilites == ( 12, 28 )
	// Chance == ( 7, 22, 36 )
	// Community Chest == ( 2, 17, 33 )
	
	Tile* t0 = new Tile( "Go", -200 );
	Tile* t1 = new Tile( "Mediterranean Avenue", 60 );
	Tile* t2 = new Tile( "Community Chest 0", 0 );
	Tile* t3 = new Tile( "Baltic Avenue", 60 );
	Tile* t4 = new Tile( "Income Tax", 200 );
	Tile* t5 = new Tile( "Reading Railroad", 200 );
	Tile* t6 = new Tile( "Oriental Avenue", 100 );
	Tile* t7 = new Tile( "Chance 0", 0 );
	Tile* t8 = new Tile( "Vermont avenue", 100 );
	Tile* t9 = new Tile( "Connecticut Avenue", 120 );
	Tile* t10 = new Tile( "Jail", 0 );
	Tile* t11 = new Tile( "St. Charcles Place", 140 );
	Tile* t12 = new Tile( "Electric Company", 150 );
	Tile* t13 = new Tile( "States Avenue", 140 );
	Tile* t14 = new Tile( "Virginia Avenue", 160 );
	Tile* t15 = new Tile( "Pennsylvania Railroad", 200 );
	Tile* t16 = new Tile( "St. James Place", 180 );
	Tile* t17 = new Tile( "Community Chest 1", 0 );
	Tile* t18 = new Tile( "Tennessee Avenue", 180 );
	Tile* t19 = new Tile( "New York Avenue", 200 );
	Tile* t20 = new Tile( "Free Parking", 0 );
	Tile* t21 = new Tile( "Kentucky Avenue", 220 );
	Tile* t22 = new Tile( "Chance 1", 0 );
	Tile* t23 = new Tile( "Indiana Avenue", 220 );
	Tile* t24 = new Tile( "Illinois Avenue", 240 );
	Tile* t25 = new Tile( "B. & O. Railroad", 200 );
	Tile* t26 = new Tile( "Atlantic Avenue", 260 );
	Tile* t27 = new Tile( "Ventor Avenue", 260 );
	Tile* t28 = new Tile( "Water Works", 150 );
	Tile* t29 = new Tile( "Marvin Gardens", 280 );
	Tile* t30 = new Tile( "Go To Jail", 0 );
	Tile* t31 = new Tile( "Pacific Avenue", 300 );
	Tile* t32 = new Tile( "North Carolina Avenue", 300 );
	Tile* t33 = new Tile( "Community Chest 2", 0 );
	Tile* t34 = new Tile( "Pennsylvania Avenue", 320 );
	Tile* t35 = new Tile( "Short Line", 200 );
	Tile* t36 = new Tile( "Chance 2", 0 );
	Tile* t37 = new Tile( "Park Place", 350 );
	Tile* t38 = new Tile( "Luxury Tax", 100 );
	Tile* t39 = new Tile( "Boardwalk", 400 );

	p_tilePos.insert( std::pair<int, Tile*>( 0, t0 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 1, t1 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 2, t2 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 3, t3 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 4, t4 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 5, t5 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 6, t6 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 7, t7 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 8, t8 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 9, t9 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 10, t10 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 11, t11 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 12, t12 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 13, t13 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 14, t14 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 15, t15 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 16, t16 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 17, t17 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 18, t18 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 19, t19 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 20, t20 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 21, t21 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 22, t22 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 23, t23 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 24, t24 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 25, t25 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 26, t26 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 27, t27 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 28, t28 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 29, t29 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 30, t30 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 31, t31 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 32, t32 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 33, t33 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 34, t34 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 35, t35 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 36, t36 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 37, t37 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 38, t38 ) );
	p_tilePos.insert( std::pair<int, Tile*>( 39, t39 ) );
	
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 0 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 1 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 2 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 3 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 4 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 5 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 6 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 7 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 8 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 9 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 10 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 11 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 12 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 13 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 14 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 15 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 16 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 17 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 18 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 19 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 20 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 21 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 22 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 23 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 24 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 25 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 26 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 27 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 28 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 29 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 30 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 31 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 32 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 33 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 34 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 35 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 36 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 37 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 38 ], 0 ) );
	p_tileLands.insert( std::pair<Tile*, unsigned long>( p_tilePos[ 39 ], 0 ) );
}

