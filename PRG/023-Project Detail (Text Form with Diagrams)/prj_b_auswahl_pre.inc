<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_b_auswahl_pre.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if((!$err && ($_POST['Button_Submit'])) || ($err && ($_POST['Button_okay'] || $_POST['Button_Loeschen'] || $_POST['Button_Anpassen'])))
{
	$Button_Submit_nv=true;

	$Projektart_sf=2;
	$Elter_sf=2;
}
else
	$Button_Zurueck_nv=true;

// Beim Ändern darf kein untergeordetes Projekt vom gewählten Projekt als Elter zur
// Auswahl stehen => sonst Zirkelbeziehung!!!
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	$GLOBALS['fxptdata']['backup']['elter_array']['Backup']['Projekt_IDopen']=$vAPID;

// Projektart (Projektart)
$Projektart_ds=0;
$maskenfeldwerte['projektart']=$vPART;

// Zugeordnet zu (Elter)
$maskenfeldwerte['elter']=$vEPID;
?>