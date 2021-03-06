//
// (C) Jan de Vaan 2007-2010, all rights reserved. See the accompanying "License.txt" for licensed use.
//

#ifndef CHARLS_CONTEXTRUNMODE
#define CHARLS_CONTEXTRUNMODE

#include <cstdint>

// Implements statistical modeling for the run mode context.
// Computes model dependent parameters like the Golomb code lengths
struct CContextRunMode
{
    // Note: members are sorted based on their size.
    int32_t A;
    int32_t _nRItype;
    uint8_t _nReset;
    uint8_t N;
    uint8_t Nn;

    CContextRunMode() noexcept
        : A(),
          _nRItype(),
          _nReset(),
          N(),
          Nn()
    {
    }


    CContextRunMode(int32_t a, int32_t nRItype, int32_t nReset) noexcept
        : A(a),
          _nRItype(nRItype),
          _nReset(static_cast<uint8_t>(nReset)),
          N(1),
          Nn(0)
    {
    }


    FORCE_INLINE int32_t GetGolomb() const noexcept
    {
        const int32_t TEMP = A + (N >> 1) * _nRItype;
        int32_t Ntest = N;
        int32_t k = 0;
        for (; Ntest < TEMP; k++)
        {
            Ntest <<= 1;
            ASSERT(k <= 32);
        }
        return k;
    }


    void UpdateVariables(int32_t Errval, int32_t EMErrval) noexcept
    {
        if (Errval < 0)
        {
            Nn = Nn + 1;
        }
        A = A + ((EMErrval + 1 - _nRItype) >> 1);
        if (N == _nReset)
        {
            A = A >> 1;
            N = N >> 1;
            Nn = Nn >> 1;
        }
        N = N + 1;
    }


    FORCE_INLINE int32_t ComputeErrVal(int32_t temp, int32_t k) const noexcept
    {
        const bool map = temp & 1;
        const int32_t errvalabs = (temp + static_cast<int32_t>(map)) / 2;

        if ((k != 0 || (2 * Nn >= N)) == map)
        {
            ASSERT(map == ComputeMap(-errvalabs, k));
            return -errvalabs;
        }

        ASSERT(map == ComputeMap(errvalabs, k));
        return errvalabs;
    }


    bool ComputeMap(int32_t Errval, int32_t k) const noexcept
    {
        if ((k == 0) && (Errval > 0) && (2 * Nn < N))
            return true;

        if ((Errval < 0) && (2 * Nn >= N))
            return true;

        if ((Errval < 0) && (k != 0))
            return true;

        return false;
    }


    FORCE_INLINE bool ComputeMapNegativeE(int32_t k) const noexcept
    {
        return k != 0 || (2 * Nn >= N);
    }
};

#endif
