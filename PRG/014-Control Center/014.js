////////////////////////////////////////////////////////////////////////////////
// File name   : 014.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-11-03                                                   //
// Last Change : 2020-11-03                                                   //
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
 * PF 14: Control Center
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init14()
{
//alert('Init Start: '+oFXP.tr);
	var ccmw=0;
	var ccmca=$$('[class^="maskc"]');
	if(ccmca.length)
	{
		var ffd=oID.fxframe.style.display;
		oID.fxframe.style.display='';
		for(var m=0; m<ccmca.length; m++)
		{
			var ccd=fxf_fn_getBCR(ccmca[m]);
			if((ccd.width > 440) && (ccd.width < dim.sd.swidth-440))
				ccmw=Math.max(ccmw,ccd.width);
		}
		if(ccmw > 0)
		{
			for(var m=0; m<ccmca.length; m++)
			{
				var ccd=fxf_fn_getBCR(ccmca[m]);
				if((ccd.width > 440) && (ccd.width < dim.sd.swidth-440))
				{
					ccmca[m].style.width='';
					ccmca[m].style.minWidth=ccmw+'px';
				}
			}
		}
		oID.fxframe.style.display=ffd;
	}
}
