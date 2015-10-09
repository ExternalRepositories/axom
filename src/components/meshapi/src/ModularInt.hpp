/**
 * \file
 *
 * \brief A simple class to handle arithmetic on modular integers
 * \detail Modular integers encapsulates a
 *         We allow the max number to be set during compile time or at runtime
 *
 */

#ifndef MESHAPI_MODULAR_INT_H_
#define MESHAPI_MODULAR_INT_H_

#include "slic/slic.hpp"
#include "meshapi/SizePolicies.hpp"


namespace asctoolkit {
namespace meshapi {


    /**
     * \class
     * This class is a wrapper around an int and encapsulates modular arithmetic with a given modulus,
     * and can be useful e.g. when we are iterating circularly through the elements in a relation (e.g. consecutive edges around a polygon)
     * The class invariant is that 0 <= val < modulus(), where val is the wrapped integer.
     * The modulus cam
     */
    template< typename SizePolicy          = policies::RuntimeSizeHolder<int> >
    class ModularInt : private SizePolicy
    {
    public:
        ModularInt(int val = 0, int modulusVal = SizePolicy::DEFAULT_VALUE) : SizePolicy(modulusVal), m_val(val)
        {
            SLIC_ASSERT( modulus() != 0);
            normalize();
        }

        ModularInt(const ModularInt& zn) : SizePolicy( zn ), m_val( zn.m_val)
        {
            SLIC_ASSERT( modulus() != 0);
            normalize();
        }


        /**
         * Implicit cast to an int
         */
        operator int() const { return m_val; }

        int modulus() const { return SizePolicy::size(); }

        ModularInt&       operator++()          { add(1); return *this; }
        const ModularInt  operator++(int)       { ModularInt tmp(m_val, modulus()); add(1); return tmp; }

        ModularInt&       operator--()          { subtract(1); return *this; }
        const ModularInt  operator--(int)       { ModularInt tmp(m_val, modulus()); subtract(1); return tmp; }

        ModularInt&       operator+=(int val)   { add(val); return *this; }
        ModularInt&       operator-=(int val)   { subtract(val); return *this; }
        ModularInt&       operator*=(int val)   { multiply(val); return *this; }

    private:
        void add(int val)       { m_val += val; normalize();}
        void subtract(int val)  { m_val -= val; normalize();}
        void multiply(int val)  { m_val *= val; normalize();}

        /**
         * Normalizes to invariant for ModularInt. Namely: 0 <= m_val < size()
         */
        void normalize()
        {
            const int sz = modulus();

        #if MODINT_BRANCHLESS
            // This solution avoids branching (at the expense of a second mod operation), but appears to be slower on chaos
            m_val = (sz + (m_val % sz)) % sz;

        #elif MODINT_STRAIGHTFORWARD
            // Straightforward solution -- possibly adds sz to ensure that m_val is non-negative -- which involves a branch
            m_val %= sz;
            if(m_val < 0)
                m_val += sz;
        #else // MODINT_MODLESS
            // this version assumes that we are usually only adding small offsets to avoid the div
//            if(m_val >= 0)
                while(m_val >= sz) m_val -= sz;
//            else
                while(m_val < 0)   m_val += sz;
        #endif

            verifyValue();
        }


        void verifyValue()
        {
            SLIC_ASSERT_MSG( m_val >= 0 && m_val < modulus()
                            , "ModularInt: Value must be between 0 and " << modulus()
                              << " but value was " << m_val <<".");
        }

    private:
        int m_val;

    };

    template<typename SizePolicy>
    ModularInt<SizePolicy> operator+(const ModularInt<SizePolicy>& zn, const int n)
    {
        ModularInt<SizePolicy> tmp(zn);
        tmp += n;
        return tmp;
    }

    template<typename SizePolicy>
    ModularInt<SizePolicy> operator+(const int n, const ModularInt<SizePolicy>& zn)
    {
        ModularInt<SizePolicy> tmp(zn);
        tmp += n;
        return tmp;
    }

    template<typename SizePolicy>
    ModularInt<SizePolicy> operator-(const ModularInt<SizePolicy>& zn, const int n)
    {
        ModularInt<SizePolicy> tmp(zn);
        tmp -= n;
        return tmp;
    }

    template<typename SizePolicy>
    ModularInt<SizePolicy> operator-(const int n, const ModularInt<SizePolicy>& zn)
    {
        ModularInt<SizePolicy> tmp(zn);
        tmp -= n;
        return tmp;
    }

    template<typename SizePolicy>
    ModularInt<SizePolicy> operator*(const ModularInt<SizePolicy>& zn, const int n)
    {
        ModularInt<SizePolicy> tmp(zn);
        tmp *= n;
        return tmp;
    }

    template<typename SizePolicy>
    ModularInt<SizePolicy> operator*(const int n, const ModularInt<SizePolicy>& zn)
    {
        ModularInt<SizePolicy> tmp(zn);
        tmp *= n;
        return tmp;
    }



} // end namespace meshapi
} // end namespace asctoolkit

#endif //  MESHAPI_MODULAR_INT_H_
