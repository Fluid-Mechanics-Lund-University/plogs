/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "reactionTypes.H"
#include "makeReaction.H"

#include "ArrheniusPLOGReactionRate.H"
#include "infiniteReactionRate.H"
#include "LandauTellerReactionRate.H"
#include "thirdBodyArrheniusReactionRate.H"

#include "ChemicallyActivatedReactionRate.H"
#include "JanevReactionRate.H"
#include "powerSeriesReactionRate.H"

#include "FallOffReactionRate.H"
#include "LindemannFallOffFunction.H"
#include "SRIFallOffFunction.H"
#include "TroeFallOffFunction.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeReactions(Thermo, Reaction)                                        \
                                                                               \
    defineTemplateTypeNameAndDebug(Reaction, 0);                               \
    defineTemplateRunTimeSelectionTable(Reaction, dictionary);                 \
                                                                               \
    makeIRNReactions(Thermo, ArrheniusPLOGReactionRate)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    // sensible enthalpy based reactions
    makeReactions(constGasHThermoPhysics, constGasHReaction)
    makeReactions(gasHThermoPhysics, gasHReaction)
    makeReactions
    (
        constIncompressibleGasHThermoPhysics,
        constIncompressibleGasHReaction
    )
    makeReactions(incompressibleGasHThermoPhysics, incompressibleGasHReaction)
    makeReactions(icoPoly8HThermoPhysics, icoPoly8HReaction)
    makeReactions(constFluidHThermoPhysics, constFluidHReaction)
    makeReactions
    (
        constAdiabaticFluidHThermoPhysics,
        constAdiabaticFluidHReaction
    )
    makeReactions(constHThermoPhysics, constHReaction)

    makeReactions(constGasEThermoPhysics, constGasEReaction)
    makeReactions(gasEThermoPhysics, gasEReaction)
    makeReactions
    (
        constIncompressibleGasEThermoPhysics,
        constIncompressibleGasEReaction
    )
    makeReactions(incompressibleGasEThermoPhysics, incompressibleGasEReaction)
    makeReactions(icoPoly8EThermoPhysics, icoPoly8EReaction)
    makeReactions(constFluidEThermoPhysics, constFluidEReaction)
    makeReactions
    (
        constAdiabaticFluidEThermoPhysics,
        constAdiabaticFluidEReaction
    )
    makeReactions(constEThermoPhysics, constEReaction)
}

// ************************************************************************* //
