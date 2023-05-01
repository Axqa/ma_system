#ifndef ENUMS_H
#define ENUMS_H

enum CoordSystemT {
  CARTESIAN,
  POLAR
};

/*!The SideT enumeration contains the two sides */
enum SideT {
  SIDE_LEFT,     /*!< left side    */
  SIDE_RIGHT,    /*!< right SIDE   */
  SIDE_ILLEGAL   /*!< illegal SIDE */
} ;

/*! The ViewAngleT enumeration contains the different view angles that are
    possible for a player */
enum ViewAngleT  {
  VA_NARROW,  /*!< view angle narrow  */
  VA_NORMAL,  /*!< view angle normal  */
  VA_WIDE,    /*!< view angle wide    */
  VA_ILLEGAL  /*!< illegal view angle */
};

/*!The ViewQualityT enumeration contains the different view qualities possible
   for a player. */
enum ViewQualityT {
  VQ_HIGH,    /*!< view quality high    */
  VQ_LOW,     /*!< view quality low     */
  VQ_ILLEGAL  /*!< illegal view quality */
};

/*!The PlayModeT enumeration contains all playmodes of the soccer playmode.
   The associated string values can be returned using the methods in the
   SoccerTypes class */
enum PlayModeT {
  PM_BEFORE_KICK_OFF,        /*!< before_kick_off:   before kick off         */
  PM_KICK_OFF_LEFT,          /*!< kick_off_l:        kick off for left team  */
  PM_KICK_OFF_RIGHT,         /*!< kick_off_r:        kick off for right team */
  PM_KICK_IN_LEFT,           /*!< kick_in_l:         kick in for left team   */
  PM_KICK_IN_RIGHT,          /*!< kick_in_r:         kick in for right team  */
  PM_CORNER_KICK_LEFT,       /*!< corner_kick_l:     corner kick left team   */
  PM_CORNER_KICK_RIGHT,      /*!< corner_kick_r:     corner kick right team  */
  PM_GOAL_KICK_LEFT,         /*!< goal_kick_l:       goal kick for left team */
  PM_GOAL_KICK_RIGHT,        /*!< goal_kick_r:       goal kick for right team*/
  PM_GOAL_LEFT,              /*!< goal_l:            goal scored by team left*/
  PM_GOAL_RIGHT,             /*!< goal_r:            goal scored by team righ*/
  PM_FREE_KICK_FAULT_LEFT,   /*!< free_kick_fault_l: free_kick to yourself   */
  PM_FREE_KICK_FAULT_RIGHT,	 /*!< free_kick_fault_r: free_kick to self   */
  PM_FREE_KICK_LEFT,         /*!< free_kick_l:       free kick for left team */
  PM_FREE_KICK_RIGHT,        /*!< free_kick_r:       free kick for right team*/
  PM_INDIRECT_FREE_KICK_RIGHT,/*!<indirect_free_kick_r: ind. free kick right */
  PM_INDIRECT_FREE_KICK_LEFT,/*!< indirect_free_kick_l: ind. free kick left  */
  PM_BACK_PASS_LEFT,         /*!< back_pass_l:       left team passed back   */
  PM_BACK_PASS_RIGHT,        /*!< back_pass_r:       right team passed back  */
  PM_OFFSIDE_LEFT,           /*!< offside_l:         offside for left team   */
  PM_OFFSIDE_RIGHT,          /*!< offside_r:         offside for right team  */
  PM_PLAY_ON,                /*!< play_on:           play on (during match)  */
  PM_TIME_OVER,              /*!< time_over:         time over (after match) */
  PM_PENALTY_SETUP_LEFT,     /*!< penalty_setup_left left setups for penalty */
  PM_PENALTY_SETUP_RIGHT,    /*!< penalty_setup_right right setup for penalty*/
  PM_PENALTY_READY_LEFT,     /*!< penalty_ready_left ready for penalty l team*/
  PM_PENALTY_READY_RIGHT,    /*!< penalty_ready_right ready for pen r team   */
  PM_PENALTY_TAKEN_LEFT,     /*!< penalty_taken_left  penalty started left   */
  PM_PENALTY_TAKEN_RIGHT,    /*!< penalty_taken_right penalty started right  */
  PM_FROZEN,                 /*!< game play is frozen                        */
  PM_QUIT,                   /*!< quit                                       */
  PM_ILLEGAL                 /*!< unknown playmode                           */
} ;


/*!The CommandT enumeration contains the different types for the SoccerCommand
   that are possible. */
enum CommandT {
  CMD_ILLEGAL,      /*!< illegal command */
  CMD_DASH,         /*!< dash command (player only)         */
  CMD_TURN,         /*!< turn command (player only)         */
  CMD_TURNNECK,     /*!< turn_neck command (player only)    */
  CMD_CHANGEVIEW,   /*!< change view command (player only)  */
  CMD_CATCH,        /*!< catch command (goalie only)        */
  CMD_KICK,         /*!< kick command (player only)         */
  CMD_MOVE,         /*!< move command                       */
  CMD_SENSEBODY,    /*!< sense_body command (player only)   */
  CMD_SAY,          /*!< say command                        */
  CMD_CHANGEPLAYER, /*!< change_player command (coach only) */
  CMD_ATTENTIONTO,  /*!< pay attention to specific player   */
  CMD_TACKLE,       /*!< tackle in current body direction   */
  CMD_POINTTO,      /*!< point arm towards a point on field */
  CMD_MAX_COMMANDS,  /*!< maximum number of commands         */
  CMD_SYNC_SEE		/* sync_see command*/
} ;

// 53 flags
enum FlagT {
    FLAG_L_T,         /*!< Flag left top            */
    FLAG_T_L_50,      /*!< Flag top left 50         */
    FLAG_T_L_40,      /*!< Flag top left 40         */
    FLAG_T_L_30,      /*!< Flag top left 30         */
    FLAG_T_L_20,      /*!< Flag top left 20         */
    FLAG_T_L_10,      /*!< Flag top left 10         */
    FLAG_T_0,         /*!< Flag top left 0          */
    FLAG_C_T,         /*!< Flag top center          */
    FLAG_T_R_10,      /*!< Flag top right 10        */
    FLAG_T_R_20,      /*!< Flag top right 20        */
    FLAG_T_R_30,      /*!< Flag top right 30        */
    FLAG_T_R_40,      /*!< Flag top right 40        */
    FLAG_T_R_50,      /*!< Flag top right 50        */
    FLAG_R_T,         /*!< Flag right top           */
    FLAG_R_T_30,      /*!< Flag right top 30        */
    FLAG_R_T_20,      /*!< Flag right top 20        */
    FLAG_R_T_10,      /*!< Flag right top 10        */
    FLAG_G_R_T,       /*!< Flag goal right top      */
    FLAG_R_0,         /*!< Flag right 0             */
    FLAG_G_R_B,       /*!< Flag goal right bottom   */
    FLAG_R_B_10,      /*!< Flag right bottom 10     */
    FLAG_R_B_20,      /*!< Flag right bottom 20     */
    FLAG_R_B_30,      /*!< Flag right bottom 30     */
    FLAG_R_B,         /*!< Flag right bottom        */
    FLAG_B_R_50,      /*!< Flag bottom right 50     */
    FLAG_B_R_40,      /*!< Flag bottom right 40     */
    FLAG_B_R_30,      /*!< Flag bottom right 30     */
    FLAG_B_R_20,      /*!< Flag bottom right 20     */
    FLAG_B_R_10,      /*!< Flag bottom right 10     */
    FLAG_C_B,         /*!< Flag center bottom       */
    FLAG_B_0,         /*!< Flag bottom 0            */
    FLAG_B_L_10,      /*!< Flag bottom left 10      */
    FLAG_B_L_20,      /*!< Flag bottom left 20      */
    FLAG_B_L_30,      /*!< Flag bottom left 30      */
    FLAG_B_L_40,      /*!< Flag bottom left 40      */
    FLAG_B_L_50,      /*!< Flag bottom left 50      */
    FLAG_L_B,         /*!< Flag left bottom         */
    FLAG_L_B_30,      /*!< Flag left bottom 30      */
    FLAG_L_B_20,      /*!< Flag left bottom 20      */
    FLAG_L_B_10,      /*!< Flag left bottom 10      */
    FLAG_G_L_B,       /*!< Flag goal left bottom    */
    FLAG_L_0,         /*!< Flag left 0              */
    FLAG_G_L_T,       /*!< Flag goal left top       */
    FLAG_L_T_10,      /*!< Flag left bottom 10      */
    FLAG_L_T_20,      /*!< Flag left bottom 20      */
    FLAG_L_T_30,      /*!< Flag left bottom 30      */
    FLAG_P_L_T,       /*!< Flag penaly left top     */
    FLAG_P_L_C,       /*!< Flag penaly left center  */
    FLAG_P_L_B,       /*!< Flag penaly left bottom  */
    FLAG_P_R_T,       /*!< Flag penaly right top    */
    FLAG_P_R_C,       /*!< Flag penaly right center */
    FLAG_P_R_B,       /*!< Flag penaly right bottom */
    FLAG_C,           /*!< Flag center field        */
};

/*! The RefereeT enumeration contains all messages that the referee can sent.
    The SoccerTypes class contains different methods to convert these messages
    to the corresponding playmode. */
enum RefereeMessageT {
  REFC_ILLEGAL,                /*!< unknown message                          */
  REFC_BEFORE_KICK_OFF,        /*!< before_kick_off:   before kick off       */
  REFC_KICK_OFF_LEFT,          /*!< kick_off_l:        kickoff for left team */
  REFC_KICK_OFF_RIGHT,         /*!< kick_off_r:        kickoff for right team*/
  REFC_KICK_IN_LEFT,           /*!< kick_in_l:         kick in for left team */
  REFC_KICK_IN_RIGHT,          /*!< kick_in_r:         kick in for right team*/
  REFC_CORNER_KICK_LEFT,       /*!< corner_kick_l:     corner kick left team */
  REFC_CORNER_KICK_RIGHT,      /*!< corner_kick_r:     corner kick right team*/
  REFC_GOAL_KICK_LEFT,         /*!< goal_kick_l:       goal kick left team   */
  REFC_GOAL_KICK_RIGHT,        /*!< goal_kick_r:       goal kick right team  */
  REFC_FREE_KICK_LEFT,         /*!< free_kick_l:       free kick left team   */
  REFC_FREE_KICK_RIGHT,        /*!< free_kick_r:       free kick right team  */
  REFC_INDIRECT_FREE_KICK_RIGHT,/*!< indirect_free_kick_r: ind freekick right*/
  REFC_INDIRECT_FREE_KICK_LEFT,/*!< indirect_free_kick_l: ind. free kick left*/
  REFC_FREE_KICK_FAULT_LEFT,   /*!< free_kick_fault_l: free_kick to yourself */
  REFC_FREE_KICK_FAULT_RIGHT,  /*!< free_kick_fault_r: free_kick to yourself */
  REFC_BACK_PASS_LEFT,         /*!< back_pass_l:       left team passed back */
  REFC_BACK_PASS_RIGHT,        /*!< back_pass_r:       right team passed back*/
  REFC_PLAY_ON,                /*!< play_on:           play on (during match)*/
  REFC_TIME_OVER,              /*!< time_over:         time over(after match)*/
  REFC_FROZEN,                 /*!< frozen:            game play is frozen   */
  REFC_QUIT,                   /*!< quit:              quit                  */
  REFC_OFFSIDE_LEFT,           /*!< offside_l          offside left          */
  REFC_OFFSIDE_RIGHT,          /*!< offside_r          offside right         */
  REFC_HALF_TIME,              /*!< half_time:         it is half time       */
  REFC_TIME_UP,                /*!< time_up:           match has finished    */
  REFC_TIME_UP_WITHOUT_A_TEAM, /*!< time_up_without_a_team: match finished   */
  REFC_TIME_EXTENDED,          /*!< time_extended:     time cont. overtime   */
  REFC_FOUL_LEFT,              /*!< foul_l:            foul made by left     */
  REFC_FOUL_RIGHT,             /*!< foul_r:            foul made by right    */
  REFC_GOAL_LEFT,              /*!< goal_l:            goal made by left     */
  REFC_GOAL_RIGHT,             /*!< goal_r:            goal made by right    */
  REFC_DROP_BALL,              /*!< drop_ball:         ball is dropped       */
  REFC_GOALIE_CATCH_BALL_LEFT, /*!< goalie_catch_ball_l: left goalie catch   */
  REFC_GOALIE_CATCH_BALL_RIGHT,/*!< goalie_catch_ball_r: right goalie catch  */
  REFC_PENALTY_SETUP_LEFT,     /*!< penalty_setup_left left setup for penalty*/
  REFC_PENALTY_SETUP_RIGHT,    /*!< penalty_setup_right right setups for pen.*/
  REFC_PENALTY_READY_LEFT,     /*!< penalty_ready_left  ready for pen. l team*/
  REFC_PENALTY_READY_RIGHT,    /*!< penalty_ready_right ready for pen. r team*/
  REFC_PENALTY_TAKEN_LEFT,     /*!< penalty_ready_left  pen. taken by  l team*/
  REFC_PENALTY_TAKEN_RIGHT,    /*!< penalty_ready_right pen. taken by  r team*/
  REFC_PENALTY_MISS_LEFT,      /*!< penalty_miss_left   penalty missed r team*/
  REFC_PENALTY_MISS_RIGHT,     /*!< penalty_miss_right  penalty missed l team*/
  REFC_PENALTY_SCORE_LEFT,     /*!< penalty_score_left  penalty score l team */
  REFC_PENALTY_SCORE_RIGHT,    /*!< penalty_score_right penalty score r team */
  REFC_PENALTY_FOUL_LEFT,      /*!< penalty_foul_left   penalty foul l team  */
  REFC_PENALTY_FOUL_RIGHT,     /*!< penalty_foul_right  penalty foul r team  */
  REFC_PENALTY_ONFIELD_LEFT,   /*!< penalty_onfield_left pen. on left field  */
  REFC_PENALTY_ONFIELD_RIGHT,  /*!< penalty_onfield_right pen. on right field*/
  REFC_PENALTY_WINNER_LEFT,    /*!< penalty_winner_l    penalty won by r team*/
  REFC_PENALTY_WINNER_RIGHT,   /*!< penalty_winner_r    penalty won by l team*/
  REFC_PENALTY_DRAW            /*!< penalty_draw        penalty result = draw*/
} ;

/*! ObjectT is an enumeration of all possible objects that are part of the
    RoboCup soccer simulation. The class SoccerTypes contains different methods
    to easily work with these objects and convert them to text strings and text
    strings to ObjectT. */
enum ObjectT { // don't change order
  OBJECT_BALL = 1,             /*!< Ball                     */
  OBJECT_GOAL_L,           /*!< Left goal                */    // 2 goals
  OBJECT_GOAL_R,           /*!< Right goal               */
  OBJECT_GOAL_UNKNOWN,     /*!< Unknown goal             */
  OBJECT_LINE_L,           /*!< Left line                */    // 4 lines
  OBJECT_LINE_R,           /*!< Right line               */
  OBJECT_LINE_B,           /*!< Bottom line              */
  OBJECT_LINE_T,           /*!< Top line                 */
  OBJECT_FLAG_L_T,         /*!< Flag left top            */   // 53 flags
  OBJECT_FLAG_T_L_50,      /*!< Flag top left 50         */
  OBJECT_FLAG_T_L_40,      /*!< Flag top left 40         */
  OBJECT_FLAG_T_L_30,      /*!< Flag top left 30         */
  OBJECT_FLAG_T_L_20,      /*!< Flag top left 20         */
  OBJECT_FLAG_T_L_10,      /*!< Flag top left 10         */
  OBJECT_FLAG_T_0,         /*!< Flag top left 0          */
  OBJECT_FLAG_C_T,         /*!< Flag top center          */
  OBJECT_FLAG_T_R_10,      /*!< Flag top right 10        */
  OBJECT_FLAG_T_R_20,      /*!< Flag top right 20        */
  OBJECT_FLAG_T_R_30,      /*!< Flag top right 30        */
  OBJECT_FLAG_T_R_40,      /*!< Flag top right 40        */
  OBJECT_FLAG_T_R_50,      /*!< Flag top right 50        */
  OBJECT_FLAG_R_T,         /*!< Flag right top           */
  OBJECT_FLAG_R_T_30,      /*!< Flag right top 30        */
  OBJECT_FLAG_R_T_20,      /*!< Flag right top 20        */
  OBJECT_FLAG_R_T_10,      /*!< Flag right top 10        */
  OBJECT_FLAG_G_R_T,       /*!< Flag goal right top      */
  OBJECT_FLAG_R_0,         /*!< Flag right 0             */
  OBJECT_FLAG_G_R_B,       /*!< Flag goal right bottom   */
  OBJECT_FLAG_R_B_10,      /*!< Flag right bottom 10     */
  OBJECT_FLAG_R_B_20,      /*!< Flag right bottom 20     */
  OBJECT_FLAG_R_B_30,      /*!< Flag right bottom 30     */
  OBJECT_FLAG_R_B,         /*!< Flag right bottom        */
  OBJECT_FLAG_B_R_50,      /*!< Flag bottom right 50     */
  OBJECT_FLAG_B_R_40,      /*!< Flag bottom right 40     */
  OBJECT_FLAG_B_R_30,      /*!< Flag bottom right 30     */
  OBJECT_FLAG_B_R_20,      /*!< Flag bottom right 20     */
  OBJECT_FLAG_B_R_10,      /*!< Flag bottom right 10     */
  OBJECT_FLAG_C_B,         /*!< Flag center bottom       */
  OBJECT_FLAG_B_0,         /*!< Flag bottom 0            */
  OBJECT_FLAG_B_L_10,      /*!< Flag bottom left 10      */
  OBJECT_FLAG_B_L_20,      /*!< Flag bottom left 20      */
  OBJECT_FLAG_B_L_30,      /*!< Flag bottom left 30      */
  OBJECT_FLAG_B_L_40,      /*!< Flag bottom left 40      */
  OBJECT_FLAG_B_L_50,      /*!< Flag bottom left 50      */
  OBJECT_FLAG_L_B,         /*!< Flag left bottom         */
  OBJECT_FLAG_L_B_30,      /*!< Flag left bottom 30      */
  OBJECT_FLAG_L_B_20,      /*!< Flag left bottom 20      */
  OBJECT_FLAG_L_B_10,      /*!< Flag left bottom 10      */
  OBJECT_FLAG_G_L_B,       /*!< Flag goal left bottom    */
  OBJECT_FLAG_L_0,         /*!< Flag left 0              */
  OBJECT_FLAG_G_L_T,       /*!< Flag goal left top       */
  OBJECT_FLAG_L_T_10,      /*!< Flag left bottom 10      */
  OBJECT_FLAG_L_T_20,      /*!< Flag left bottom 20      */
  OBJECT_FLAG_L_T_30,      /*!< Flag left bottom 30      */
  OBJECT_FLAG_P_L_T,       /*!< Flag penaly left top     */
  OBJECT_FLAG_P_L_C,       /*!< Flag penaly left center  */
  OBJECT_FLAG_P_L_B,       /*!< Flag penaly left bottom  */
  OBJECT_FLAG_P_R_T,       /*!< Flag penaly right top    */
  OBJECT_FLAG_P_R_C,       /*!< Flag penaly right center */
  OBJECT_FLAG_P_R_B,       /*!< Flag penaly right bottom */
  OBJECT_FLAG_C,           /*!< Flag center field        */
  OBJECT_TEAMMATE_1,       /*!< Teammate nr 1 =72           */    // teammates 61
  OBJECT_TEAMMATE_2,       /*!< Teammate nr 2            */
  OBJECT_TEAMMATE_3,       /*!< Teammate nr 3            */
  OBJECT_TEAMMATE_4,       /*!< Teammate nr 4            */
  OBJECT_TEAMMATE_5,       /*!< Teammate nr 5            */
  OBJECT_TEAMMATE_6,       /*!< Teammate nr 6            */
  OBJECT_TEAMMATE_7,       /*!< Teammate nr 7            */
  OBJECT_TEAMMATE_8,       /*!< Teammate nr 8            */
  OBJECT_TEAMMATE_9,       /*!< Teammate nr 9            */
  OBJECT_TEAMMATE_10,      /*!< Teammate nr 10           */
  OBJECT_TEAMMATE_11,      /*!< Teammate nr 11           */
  OBJECT_TEAMMATE_UNKNOWN, /*!< Teammate nr unkown       */
  OBJECT_OPPONENT_1,       /*!< Opponent nr 1            */    // opponents 73
  OBJECT_OPPONENT_2,       /*!< Opponent nr 2            */
  OBJECT_OPPONENT_3,       /*!< Opponent nr 3            */
  OBJECT_OPPONENT_4,       /*!< Opponent nr 4            */
  OBJECT_OPPONENT_5,       /*!< Opponent nr 5            */
  OBJECT_OPPONENT_6,       /*!< Opponent nr 6            */
  OBJECT_OPPONENT_7,       /*!< Opponent nr 7            */
  OBJECT_OPPONENT_8,       /*!< Opponent nr 8            */
  OBJECT_OPPONENT_9,       /*!< Opponent nr 9            */
  OBJECT_OPPONENT_10,      /*!< Opponent nr 10           */
  OBJECT_OPPONENT_11,      /*!< Opponent nr 11           */
  OBJECT_OPPONENT_UNKNOWN, /*!< Opponent nr unknown      */    // 84
  OBJECT_PLAYER_UNKNOWN,   /*!< Unknown player           */
  OBJECT_UNKNOWN,          /*!< Unknown object           */
  OBJECT_TEAMMATE_GOALIE,  /*!< Goalie of your side      */
  OBJECT_OPPONENT_GOALIE,  /*!< Goalie of opponent side  */
  OBJECT_ILLEGAL,          /*!< illegal object           */
  OBJECT_MAX_OBJECTS       /*!< maximum nr of objects    */ // 90
} ;

enum MoveT {
    OBSERVE,
    LINE,
    SQUARE
};

#endif // ENUMS_H
