/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
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

Class
    Foam::ArrheniusPLOGReactionRate

Description
    Arrhenius reaction rate given by:

        k = A * T^beta * exp(-Ta/T)

SourceFiles
    ArrheniusPLOGReactionRateI.H

\*---------------------------------------------------------------------------*/

#ifndef ArrheniusPLOGReactionRate_H
#define ArrheniusPLOGReactionRate_H

#include "scalarField.H"
#include "typeInfo.H"
#include "SortableList.H" 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

class ArrheniusPLOGReactionRate;

Ostream& operator<<(Ostream&, const ArrheniusPLOGReactionRate&);


/*---------------------------------------------------------------------------*\
                    Class ArrheniusPLOGReactionRate Declaration
\*---------------------------------------------------------------------------*/

class ArrheniusPLOGReactionRate
{
    // Private Data

        scalar A_;
        scalar beta_;
        scalar Ta_;

    // Helper class to store specie-local volatile data
    class ArrheniusData
        {
            // Private Data

                //- pressure range
                scalar p_;

                scalar A_;
                scalar beta_;
                scalar Ta_;

        public:

            // Constructors

                //- Null constructor
                ArrheniusData()
                :
                    p_(0.0),
                    A_(0.0),
                    beta_(0.0),
                    Ta_(0.0)
                {}

                //- Construct from Istream
                ArrheniusData(Istream& is)
                :
                    p_(readScalar(is)),
                    A_(readScalar(is)),
                    beta_(readScalar(is)),
                    Ta_(readScalar(is))
                {}
    /*
                //- Construct from a copy of ArrheniusData
                ArrheniusData(ArrheniusData& pcm)
                :
                    p_(pcm.p()),
                    A_(pcm.A()),
                    beta_(pcm.beta()),
                    Ta_(pcm.Ta())
                {}
    */
            //- Destructor
            ~ArrheniusData()
            {}


        // Public Member Functions

            // Access

                //- Return const access to the name
                const scalar& p() const
                {
                    return p_;
                }

                //- Return const access to the pre-exponential factor
                scalar A() const
                {
                    return A_;
                }

                //- Return const access to beta
                scalar beta() const
                {
                    return beta_;
                }

                //- Return const access to the activation energy
                scalar Ta() const
                {
                    return Ta_;
                }


        // IOstream Operators

            //- Read from Istream
            friend Istream& operator>>(Istream& is, ArrheniusData& ad)
            {
                is.readBeginList("ArrheniusData");
                is  >>  ad.p_  >> ad.A_  >> ad.beta_ >> ad.Ta_;
                is.readEndList("ArrheniusData");

                return is;
            }

            //- Write to Ostream
            friend Ostream& operator<<(Ostream& os, const ArrheniusData& ad)
            {
                os  << token::BEGIN_LIST
                    << ad.p_ << token::SPACE
                    << ad.A_ << token::SPACE
                    << ad.beta_ << token::SPACE
                    << ad.Ta_
                    << token::END_LIST;

                return os;
            }
    };

    // Private Data
    // Model constants
        //- List of ArrheniusData data - (p A bata Ta)
        List<ArrheniusData> ArrheniusData_;
 

public:

    // Constructors

        //- Construct from components
        inline ArrheniusPLOGReactionRate
        (
            const scalar A,
            const scalar beta,
            const scalar Ta,
            const List<ArrheniusData> ads
        );

        //- Construct from dictionary
        inline ArrheniusPLOGReactionRate
        (
            const speciesTable& species,
            const dictionary& dict
        );


    // Member Functions

        //- Return the type name
        static word type()
        {
            return "ArrheniusPLOG";
        }

        inline scalar operator()
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const;

        inline scalar ddT
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const;

        //- Third-body efficiencies (beta = 1-alpha)
        //  non-empty only for third-body reactions
        //  with enhanced molecularity (alpha != 1)
        inline const List<Tuple2<label, scalar>>& beta() const;

        //- Species concentration derivative of the pressure dependent term
        //  By default this value is 1 as it multiplies the third-body term
        inline void dcidc
        (
            const scalar p,
            const scalar T,
            const scalarField& c,
            scalarField& dcidc
        ) const;

        //- Temperature derivative of the pressure dependent term
        //  By default this value is 0 since ddT of molecularity is approx.0
        inline scalar dcidT
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const;

        //- Write to stream
        inline void write(Ostream& os) const;


    // Ostream Operator

        inline friend Ostream& operator<<
        (
            Ostream&,
            const ArrheniusPLOGReactionRate&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ArrheniusPLOGReactionRateI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
