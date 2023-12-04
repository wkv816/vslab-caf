import java.util.ArrayList;
import java.util.List;

public class hmm2 {
    

    public static void main(String[] args) {
        long number = 123456789; // Replace with your number

        List<Long> primeFactors = factorize(number);

        System.out.println("Prime factorization of " + number + ": " + primeFactors);
    }

    private static List<Long> factorize(long n) {
        List<Long> factors = new ArrayList<>();

        if (n < 2) {
            factors.add(n);
            return factors;
        }

        long x = 2, y = 2, d = 1;
        long c = 1; // Constant factor, you can experiment with different values

        while (d == 1) {
            x = f(x, c, n);
            y = f(f(y, c, n), c, n);
            d = gcd(Math.abs(x - y), n);

            if (d != 1 && d != n) {
                factors.add(d);
                n /= d;
            }
        }

        if (n > 1) {
            factors.add(n);
        }

        return factors;
    }

    private static long f(long x, long c, long n) {
        return (x * x + c) % n;
    }

    private static long gcd(long a, long b) {
        while (b != 0) {
            long temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }


    
}
