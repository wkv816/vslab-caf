import java.util.ArrayList;
//import com.google.common.math.IntMath;  

public class hmm {


    static ArrayList<Integer> list= new ArrayList<>();

    public static int pollard_Rho(int n, int zufall ) {
        int x =  generate_Random_Nr(n, zufall);
        int y =  x;
        int p = 1;

        while (p == 1) {
            x = (x*x + 1) % n;
            y = (y*y + 1) % n;
            y = (y*y + 1) % n;
            p = findGCD(Math.abs(x - y), n);
        }
        
        return p;
    }

    public static int generate_Random_Nr(int n, int zufall ) {
        int max= n;
        int min = 1;
        int range = max - min + 1;

        int x =  (int)(Math.random() * range) + min;

        return x;
    }

    private static int findGCD(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    static  boolean isPrime(int num)
    {
        if(num<=1)
        {
            return false;
        }
       for(int i=2;i<=num/2;i++)
       {
           if((num%i)==0)
               return  false;
       }
       return true;
    }

    public static ArrayList<Integer> client(int n){


         
        
        int ruckgabe = pollard_Rho(n, 3);




        //Behavior
        // Recive p
        int p = 0;

        int NdurchP = n/p;

        boolean boolndurchP = isPrime(NdurchP);

        boolean boolp= isPrime(p);

        if(boolndurchP){
            list.add(NdurchP);
        }else{
            pollard_Rho(NdurchP, 5);
        }

        if(boolp){
            list.add(p);
        }else{
            pollard_Rho(p, 5);
        }
        

        







        return list;

    }


    private static ArrayList<Integer> findprimefactor(int n) {

        /*
        ArrayList<Integer> primefactor = new ArrayList<>();
        int a=1;
        int ruckgabe;
        int remainder;
        //primefactor.add(ruckgabe);

        do {
            ruckgabe=pollard_Rho(n, a);
            remainder=n/ruckgabe;
            if (ruckgabe!=n) {
                
            }
            a=a+1;
            primefactor.add(ruckgabe);
        } while (ruckgabe==n || !isPrime(remainder));
        
        
        
        
        */


        

        return null;

    }



    public static void main(String[] args) {


        System.out.println("primr factor of 21 is " + client(609) + "!");
    }
}