<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_vergabe_strg_ini.inc                                  //
// Version     : 24.1                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$prjdoc=0;
if($Rechte_Ref && $ID_Referenz)
{
	// Prüfen, ob gewähltes Dokument ein Projektdokument ist
	$sql="SELECT elter_art, elter FROM d_dokumente WHERE dok_id=".$ID_Referenz." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$typ=db_values($sql);
	if(fxIsArray($typ) && ($typ['elter_art'] == 3))
		$prjdoc=(int)$typ['elter'];

	// Bereits eingetragene Benutzergruppen / Benutzer / Abteilungen / Projekte ermitteln
	$sql  = "SELECT gruppeart, benutzergr_id, aktionscode FROM rechte_referenz";
	$sql .= " WHERE id_referenz=".$ID_Referenz." AND rechte_ref=".$Rechte_Ref." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$arr=db_values($sql,'*');
	if(fxIsArray($arr))
	{
		foreach($arr as $a)
			$vorh_referenzen[$a['gruppeart']][$a['benutzergr_id']]=$a['aktionscode'];
	}
	else
		$vorh_referenzen=false;
}
else
	$vorh_referenzen=false;

if($ID_Referenz && ($_GET['aus_trans'] == 106))
	$ID_Referenz_bak=$ID_Referenz;
protectVar($ID_Referenz_bak,'ID_Referenz_bak');
?>