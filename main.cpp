#include <cstdint>
#include <cmath>
#include <vector>
#include <iostream>
#include <limits>
#include <iomanip>
#include <numeric>
#include <set>
#include <map>


class Fraction
{
public:
    Fraction(const double numerator = 1, const double denominator = 1) :
        numerator(numerator), denominator(denominator)
    {
    }

    bool operator==(Fraction& fraction)
    {
        fraction.reduce();
        this->reduce();
        return (this->numerator == fraction.numerator and this->denominator == fraction.denominator);
    }

    double getAsDouble() const
    {
        return (double)numerator / (double)denominator;
    }

    void reduce()
    {
        if (!reduced)
        {
            int32_t gcd = std::gcd(this->numerator, this->denominator);
            this->numerator /= gcd;
            this->denominator /= gcd;
            reduced = true;
        }
    }

    friend bool operator>(const Fraction& left, const Fraction& right)
    {
        int32_t c = left.numerator * right.denominator - left.denominator * right.numerator;
        return (c > 0);
    }

    friend bool operator<(const Fraction& left, const Fraction& right)
    {
        int32_t c = left.numerator * right.denominator - left.denominator * right.numerator;
        return (c < 0);
    }

    Fraction operator+(const Fraction& fraction) const
    {
        int32_t den = this->denominator * fraction.denominator;
        return Fraction(this->numerator * fraction.denominator + fraction.numerator * this->denominator, den);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Fraction& fraction)
    {
        stream << fraction.numerator << "/" << fraction.denominator;
        return stream;
    }

public:
    int32_t numerator;
    int32_t denominator;
    bool reduced = {false};

};


class Generator
{
public:
    Generator(const Fraction& shift, const Fraction& period) :
        m_shift(shift), m_period(period)
    {
    }

    void assignInterval(const uint32_t start, const uint32_t end)
    {
        m_interval.first = Fraction(start, 1);
        m_interval.second = Fraction(end, 1);
    }

    bool isInInterval(const Fraction& fraction) const
    {
        return fraction > m_interval.first and fraction < m_interval.second;
    }

    void generate(std::set<Fraction>& set, std::vector<Fraction>& vector)
    {
        // deduce start period
        Fraction iter = m_shift;
        while (iter < m_interval.first)
            iter = iter + m_period;

        iter.reduce();

        //load the set
        do
        {
            set.insert(iter);
            vector.push_back(iter);
            iter = iter + m_period;
            iter.reduce();
        }
        while (iter < m_interval.second);
    }

private:
    Fraction m_shift;
    Fraction m_period;

    std::pair<Fraction, Fraction> m_interval;

};


class Result
{
public:
    Result(const int32_t start, const int32_t end) :
        m_intervalStart(start), m_intervalEnd(end)
    {
    }

    void loadGenerator(const Generator& generator)
    {
        m_generators.push_back(generator);
    }

    void compute()
    {
        for (auto& generator : m_generators)
        {
            generator.assignInterval(0, 12);
            generator.generate(m_times, m_controlTimes);
        }
    }

    void printResult() const
    {
        std::cout << "All times: " << m_controlTimes.size() << '\n';
        std::cout << "Without duplication: " << m_times.size() << '\n';
    }

    void printSet() const
    {
        uint32_t index = 0;
        for (const auto& iter : m_times)
        {
            std::cout << "[" << ++index << "]: " << iter << '\n';
        }
    }

    void printAll() const
    {
        uint32_t index = 0;
        for (const auto& iter : m_controlTimes)
        {
            std::cout << "[" << ++index << "]: " << iter << '\n';
        }
    }

    uint32_t getResult() const
    {
        return m_times.size();
    }

    [[nodiscard]] bool testSet() const
    {
        std::set<double> testSet;
        for (const auto& iter : m_controlTimes)
        {
            testSet.insert(iter.getAsDouble());
        }
        return testSet.size() == m_times.size();
    }

    void printDuplicates() const
    {
        typedef std::map<Fraction, unsigned int> CounterMap;
        CounterMap counts;
        for (int i = 0; i < m_controlTimes.size(); ++i)
        {
            CounterMap::iterator it(counts.find(m_controlTimes[i]));
            if (it != counts.end()){
                it->second++;
            } else {
                counts[m_controlTimes[i]] = 1;
            }
            if (it->second == 2)
                std::cout << m_controlTimes[i] << '\n';
        }
    }


private:
    std::vector<Generator> m_generators;
    std::set<Fraction> m_times;
    const int32_t m_intervalStart;
    const int32_t m_intervalEnd;
    std::vector<Fraction> m_controlTimes;

};


int main()
{
    const int32_t basicPeriod = 12;

    // minute -> hour
    Generator gen0(Fraction((-1) * basicPeriod, 6 * 11), Fraction(basicPeriod, 12 - 1));
    // hour -> minute
    Generator gen1(Fraction(basicPeriod, 6 * 11), Fraction(basicPeriod, 12 - 1));
    // second -> hour
    Generator gen2(Fraction(-basicPeriod, 6 * 719), Fraction(basicPeriod, 720 - 1));
    // hour -> second
    Generator gen3(Fraction(basicPeriod, 6 * 719), Fraction(basicPeriod, 720 - 1));
    // second -> minute
    Generator gen4(Fraction(-basicPeriod, 6 * 708), Fraction(basicPeriod, 720 - 12));
    // minute -> second
    Generator gen5(Fraction(basicPeriod, 6 * 708), Fraction(basicPeriod, 720 - 12));

    Result result(0, basicPeriod);
    result.loadGenerator(gen0);
    result.loadGenerator(gen1);
    result.loadGenerator(gen2);
    result.loadGenerator(gen3);
    result.loadGenerator(gen4);
    result.loadGenerator(gen5);

    result.compute();
    result.printSet();
    //result.printAll();
    std::cout << "----------\n";
    result.printResult();
    //result.printDuplicates();
    std::cout << "For 24h: " << 2 * result.getResult() << '\n';

    return !result.testSet();
}
