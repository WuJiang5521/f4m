//
// Created by GuoZiYang on 2021/1/7.
//

#ifndef FMP_MATHUTIL_H
#define FMP_MATHUTIL_H

#include <vector>
#include <cmath>

class MathUtil {
public:
    static int lg_fact_top;
    static int pow2_top;
    static std::vector<double> lg_fact;
    static std::vector<int> pow2;

    static void update_lg_fact(int n) {
        while (n > lg_fact_top) {
            lg_fact.push_back(lg_fact[lg_fact_top] + log(lg_fact_top + 1));
            ++lg_fact_top;
        }
    }
    static void update_pow2(int n) {
        while (n > pow2_top) {
            pow2.push_back(pow2[pow2_top] * 2);
            ++pow2_top;
        }
    }

    static double lg_choose(int n, int k) {
        update_lg_fact((n > k ? n : k));
        if (n < k) {
            return 0;
        }
        return lg_fact[n] - lg_fact[k] - lg_fact[n - k];
    }
    static double lg_factorial(int n) {
        update_lg_fact(n);
        return lg_fact[n];
    }

    static int power2(int i) {
        update_pow2(i);
        return pow2[i];
    }

    static double intcost(int u) {
        double c = log(2.865064);
        double z = log(u);

        while (z > 0) {
            c += z;
            z = log(z);
        }

        return c;
    }
};



#endif //FMP_MATHUTIL_H
