//
// Modular Arithmetics
//
// cf.
//   noshi91: modint 構造体を使ってみませんか？ (C++)
//     http://noshi91.hatenablog.com/entry/2019/03/31/174006
//
//   drken: 「1000000007 で割ったあまり」の求め方を総特集！ ~ 逆元から離散対数まで ~
//     https://qiita.com/drken/items/3b4fdf0a78e7a138cd9a
//
// verified:
//   MUJIN 2018 F - チーム分け
//     https://atcoder.jp/contests/mujin-pc-2018/tasks/mujin_pc_2018_f  
// 


#include <iostream>
#include <vector>
using namespace std;


template<int MOD> struct Fp {
    long long val;
    constexpr Fp(long long v = 0) noexcept : val(v % MOD) {
        if (val < 0) v += MOD;
    }
    constexpr int getmod() { return MOD; }
    constexpr Fp operator - () const noexcept {
        return val ? MOD - val : 0;
    }
    constexpr Fp operator + (const Fp& r) const noexcept { return Fp(*this) += r; }
    constexpr Fp operator - (const Fp& r) const noexcept { return Fp(*this) -= r; }
    constexpr Fp operator * (const Fp& r) const noexcept { return Fp(*this) *= r; }
    constexpr Fp operator / (const Fp& r) const noexcept { return Fp(*this) /= r; }
    constexpr Fp& operator += (const Fp& r) noexcept {
        val += r.val;
        if (val >= MOD) val -= MOD;
        return *this;
    }
    constexpr Fp& operator -= (const Fp& r) noexcept {
        val -= r.val;
        if (val < 0) val += MOD;
        return *this;
    }
    constexpr Fp& operator *= (const Fp& r) noexcept {
        val = val * r.val % MOD;
        return *this;
    }
    constexpr Fp& operator /= (const Fp& r) noexcept {
        long long a = r.val, b = MOD, u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; swap(a, b);
            u -= t * v; swap(u, v);
        }
        val = val * u % MOD;
        if (val < 0) val += MOD;
        return *this;
    }
    constexpr bool operator == (const Fp& r) const noexcept {
        return this->val == r.val;
    }
    constexpr bool operator != (const Fp& r) const noexcept {
        return this->val != r.val;
    }
    friend constexpr ostream& operator << (ostream &os, const Fp<MOD>& x) noexcept {
        return os << x.val;
    }
    friend constexpr istream& operator >> (istream &is, Fp<MOD>& x) noexcept {
        return is >> x.val;
    }
    friend constexpr Fp<MOD> modpow(const Fp<MOD> &a, long long n) noexcept {
        if (n == 0) return 1;
        auto t = modpow(a, n / 2);
        t = t * t;
        if (n & 1) t = t * a;
        return t;
    }
};



// 二項係数ライブラリ
template<class T> struct BiCoef {
    vector<T> fact_, inv_, finv_;
    constexpr BiCoef(int n) noexcept : fact_(n, 1), inv_(n, 1), finv_(n, 1) {
        int MOD = fact_[0].getmod();
        for(int i = 2; i < n; i++){
            fact_[i] = fact_[i-1] * i;
            inv_[i] = -inv_[MOD%i] * (MOD/i);
            finv_[i] = finv_[i-1] * inv_[i];
        }
    }
    constexpr T com(int n, int k) const noexcept {
        if (n < k || n < 0 || k < 0) return 0;
        return fact_[n] * finv_[k] * finv_[n-k];
    }
    constexpr T fact(int n) const noexcept {
        if (n < 0) return 0;
        return fact_[n];
    }
    constexpr T inv(int n) const noexcept {
        if (n < 0) return 0;
        return inv_[n];
    }
    constexpr T finv(int n) const noexcept {
        if (n < 0) return 0;
        return finv_[n];
    }
};


// スターリング数 (n 個を k グループにわける、n >= k)
template<class T> struct Stirling {
    vector<vector<T> > S;
    constexpr Stirling(int MAX) noexcept : S(MAX, vector<T>(MAX, 0)) {
        S[0][0] = 1;
        for (int n = 1; n < MAX; ++n) {
            for (int k = 1; k <= n; ++k) {
                S[n][k] = S[n-1][k-1] + S[n-1][k] * k;
            }
        }
    }
    constexpr T get(int n, int k) {
        if (n < 0 || k < 0 || n < k) return 0;
        return S[n][k];
    }
};



const int MAX = 201010;
const int MOD = 998244353;
using mint = Fp<MOD>;

int main() {     
    BiCoef<mint> bc(MAX);
    int N; cin >> N;
    vector<int> a(N);
    for (int i = 0; i < N; ++i) cin >> a[i];

    // nums[v] := v 人以上 OK な人数
    vector<long long> nums(N+2, 0);
    for (int i = 0; i < N; ++i) nums[a[i]]++;
    for (int i = N; i >= 0; --i) nums[i] += nums[i+1];

    // DP
    vector<vector<mint> > dp(N+2, vector<mint>(N+1, 0));
    dp[N+1][0] = 1;
    for (long long x = N; x >= 1; --x) {
        for (long long y = 0; y <= nums[x]; ++y) {
            for (long long k = 0; k <= N; ++k) {
                long long y2 = y - x * k;
                if (y2 < 0) break;
                if (y2 > nums[x+1]) continue;
                mint choose = bc.com(nums[x] - y2, x * k);
                mint fact = bc.fact(x*k) / modpow(bc.fact(x), k) * bc.finv(k);
                dp[x][y] += dp[x+1][y2] * choose * fact;
            }
        }
    }
    cout << dp[1][N] << endl;
}
