/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2024 OpenFOAM Foundation
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

#include "ArrheniusPLOGReactionRate.H"
#include "physicoChemicalConstants.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::ArrheniusPLOGReactionRate::ArrheniusPLOGReactionRate
(
    const scalar A,
    const scalar beta,
    const scalar Ta
)
:
    beta_(beta),
    A_(A),
    Ta_(Ta),
    ArrheniusData_()
{}


inline Foam::ArrheniusPLOGReactionRate::ArrheniusPLOGReactionRate
(
    const scalar A,
    const scalar beta,
    const scalar Ta,
    const List<ArrheniusData> ads
)
:
    beta_(beta),
    A_(A),
    Ta_(Ta),
    ArrheniusData_(ads)
{}


inline Foam::ArrheniusPLOGReactionRate::ArrheniusPLOGReactionRate
(
    const speciesTable&,
    const dimensionSet& dims,
    const dictionary& dict
)
:
    beta_(dict.lookup<scalar>("beta", dimless)),
    A_(dict.lookup<scalar>("A", dims/pow(dimTemperature, beta_))),
    Ta_
    (
        dict.found("Ta") || !dict.found("Ea")
      ? dict.lookup<scalar>("Ta", dimTemperature)
      : dict.lookup<scalar>("Ea", dimEnergy/dimMoles)
       /constant::physicoChemical::RR.value()
    ),
    ArrheniusData_()
{
    if (dict.found("ArrheniusData"))
    {
        ArrheniusData_ = List<ArrheniusData>(dict.lookup("ArrheniusData"));
        
        if (ArrheniusData_.empty())
        {
            WarningInFunction
                << "ArrheniusPLOGReactionRate selected, but no PLOG defined"
                << nl << endl;
        }
        else
        {
            Info<< "Reading PLOG coefficients:" << endl;

            SortableList<scalar> pList(ArrheniusData_.size());
            List<ArrheniusData> tmpArrheniusData(ArrheniusData_.size());
            forAll(pList, i)
            {
                pList[i] = ArrheniusData_[i].p();
                tmpArrheniusData[i] = ArrheniusData_[i];
            }
            pList.sort();
            labelList idxList  = pList.indices();

            // Re-order the PLOG p, from small to large
            // this can speedup the sort/comparison process later on
            forAll(ArrheniusData_, i)
            {
                label idx = idxList[i];
                ArrheniusData_[i] = tmpArrheniusData[idx];
            }
            
            forAll(ArrheniusData_, i)
            {
                Info<< "    PLOG /"
                     << " "<< ArrheniusData_[i].p()
                     << " "<< ArrheniusData_[i].A()
                     << " "<< ArrheniusData_[i].beta()
                     << " "<< ArrheniusData_[i].Ta()
                     << " /"
                    << endl;
            } 
        }
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline void Foam::ArrheniusPLOGReactionRate::preEvaluate() const
{}


inline void Foam::ArrheniusPLOGReactionRate::postEvaluate() const
{}


inline Foam::scalar Foam::ArrheniusPLOGReactionRate::operator()
(
    const scalar p,
    const scalar T,
    const scalarField&,
    const label
) const
{
    if (ArrheniusData_.empty())
    {
        scalar k = A_;
        if (mag(beta_) > vSmall)
        {
            k *= pow(T, beta_);
        }
        if (mag(Ta_) > vSmall)
        {
            k *= exp(-Ta_/T);
        }
        return k;
    }
    else
    {
        // ArrheniusData_[i].p() must be in order
        const label length = ArrheniusData_.size();

        if(p<=ArrheniusData_[0].p())
        {
            scalar k = ArrheniusData_[0].A();
            if (mag(ArrheniusData_[0].beta()) > vSmall)
            {
                k *= pow(T, ArrheniusData_[0].beta());
            }
            if (mag(ArrheniusData_[0].Ta()) > vSmall)
            {
                k *= exp(-ArrheniusData_[0].Ta()/T);
            }
            return k;
        }
        else if(p>=ArrheniusData_[length-1].p())
        {
            scalar k = ArrheniusData_[length-1].A();
            if (mag(ArrheniusData_[length-1].beta()) > vSmall)
            {
                k *= pow(T, ArrheniusData_[length-1].beta());
            }
            if (mag(ArrheniusData_[length-1].Ta()) > vSmall)
            {
                k *= exp(-ArrheniusData_[length-1].Ta()/T);
            }
            return k;
        }
        else
        {
            // Find left and right indexes
            label high = length-1;
            label low = 0;
            label mid = (high+low)/2;
            while(high>low)
            {
                if(ArrheniusData_[mid].p() >= p)
                {
                    high = mid;
                }
                else
                {
                    low = mid+1;
                }
                mid = (high+low)/2;
                //mid = low = high and exit
            }
            label left = high-1;
            label right = high;
            scalar pl = ArrheniusData_[left].p();
            scalar kl = ArrheniusData_[left].A();
            if (mag(ArrheniusData_[left].beta()) > vSmall)
            {
                kl *= pow(T, ArrheniusData_[left].beta());
            }
            if (mag(ArrheniusData_[left].Ta()) > vSmall)
            {
                kl *= exp(-ArrheniusData_[left].Ta()/T);
            }

            scalar pr = ArrheniusData_[right].p();
            scalar kr = ArrheniusData_[right].A();
            if (mag(ArrheniusData_[right].beta()) > vSmall)
            {
                kr *= pow(T, ArrheniusData_[right].beta());
            }
            if (mag(ArrheniusData_[right].Ta()) > vSmall)
            {
                kr *= exp(-ArrheniusData_[right].Ta()/T);
            }

            scalar logk = log10(kl) + (log10(p) - log10(pl))*(log10(kr) - log10(kl))/(log10(pr) - log10(pl));
            
            scalar k=pow(10, logk);
            return k;
        }
    }
}


inline Foam::scalar Foam::ArrheniusPLOGReactionRate::ddT
(
    const scalar p,
    const scalar T,
    const scalarField&,
    const label
) const
{
    if (ArrheniusData_.empty())
    {
        scalar k = A_;
        if (mag(beta_) > vSmall)
        {
            k *= pow(T, beta_);
        }
        if (mag(Ta_) > vSmall)
        {
            k *= exp(-Ta_/T);
        }
        //ddt(k)=k*(beta_+Ta_/T)/T;
        return k*(beta_+Ta_/T)/T;
    }
    else
    {
        // ArrheniusData_[i].p() must be in order
        const label length = ArrheniusData_.size();

        if(p<=ArrheniusData_[0].p())
        {
            scalar k = ArrheniusData_[0].A();
            if (mag(ArrheniusData_[0].beta()) > vSmall)
            {
                k *= pow(T, ArrheniusData_[0].beta());
            }
            if (mag(ArrheniusData_[0].Ta()) > vSmall)
            {
                k *= exp(-ArrheniusData_[0].Ta()/T);
            }
            //ddt(k)=k*(beta_+Ta_/T)/T;
            return k*(ArrheniusData_[0].beta()+ArrheniusData_[0].Ta()/T)/T;
        }
        else if(p>=ArrheniusData_[length-1].p())
        {
            scalar k = ArrheniusData_[length-1].A();
            if (mag(ArrheniusData_[length-1].beta()) > vSmall)
            {
                k *= pow(T, ArrheniusData_[length-1].beta());
            }
            if (mag(ArrheniusData_[length-1].Ta()) > vSmall)
            {
                k *= exp(-ArrheniusData_[length-1].Ta()/T);
            }
            //ddt(k)=k*(beta_+Ta_/T)/T;
            return k*(ArrheniusData_[length-1].beta()+ArrheniusData_[length-1].Ta()/T)/T;
        }
        else
        {
            // Find left and right indexes
            label high = length-1;
            label low = 0;
            label mid = (high+low)/2;
            while(high>low)
            {
                if(ArrheniusData_[mid].p() >= p)
                {
                    high = mid;
                }
                else
                {
                    low = mid+1;
                }
                mid = (high+low)/2;
                //mid = low = high and exit
            }
            label left = high-1;
            label right = high;
            scalar pl = ArrheniusData_[left].p();
            scalar kl = ArrheniusData_[left].A();
            if (mag(ArrheniusData_[left].beta()) > vSmall)
            {
                kl *= pow(T, ArrheniusData_[left].beta());
            }
            if (mag(ArrheniusData_[left].Ta()) > vSmall)
            {
                kl *= exp(-ArrheniusData_[left].Ta()/T);
            }

            scalar pr = ArrheniusData_[right].p();
            scalar kr = ArrheniusData_[right].A();
            if (mag(ArrheniusData_[right].beta()) > vSmall)
            {
                kr *= pow(T, ArrheniusData_[right].beta());
            }
            if (mag(ArrheniusData_[right].Ta()) > vSmall)
            {
                kr *= exp(-ArrheniusData_[right].Ta()/T);
            }

            scalar logk = log10(kl) + (log10(p) - log10(pl))*(log10(kr) - log10(kl))/(log10(pr) - log10(pl));
            
            scalar k=pow(10, logk);
            
            // k = kl*((kr/kl)^((log10(p) - log10(pl))/(log10(pr) - log10(pl))))
            // i.e.
            // k = kl^((log10(pr) - log10(p))/(log10(pr) - log10(pl)))*kr((log10(p) - log10(pl))/(log10(pr) - log10(pl)))
            // a = (log10(pr) - log10(p))/(log10(pr) - log10(pl));
            // b = (log10(p) - log10(pl))/(log10(pr) - log10(pl));
            // ddT(k) = k*(kl*b+kr*a)/(kr*kl)
        
            scalar a = (log10(pr) - log10(p))/(log10(pr) - log10(pl));
            scalar b = (log10(p) - log10(pl))/(log10(pr) - log10(pl));
            
            // ddT(k) = k*(kl*b+kr*a)/(kr*kl)
            return k*(kl*b+kr*a)/(kr*kl);
        }
    }
}


inline bool Foam::ArrheniusPLOGReactionRate::hasDdc() const
{
    return false;
}


inline void Foam::ArrheniusPLOGReactionRate::ddc
(
    const scalar p,
    const scalar T,
    const scalarField& c,
    const label li,
    scalarField& ddc
) const
{
    ddc = 0;
}


inline void Foam::ArrheniusPLOGReactionRate::write(Ostream& os) const
{
    writeEntry(os, "A", A_);
    writeEntry(os, "beta", beta_);
    writeEntry(os, "Ta", Ta_);
    //writeEntry(os, "ArrheniusData", ArrheniusData_);
    forAll(ArrheniusData_, i)
    {
            os << ArrheniusData_[i];
    }
}


inline Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const ArrheniusPLOGReactionRate& arr
)
{
    arr.write(os);
    return os;
}


// ************************************************************************* //
