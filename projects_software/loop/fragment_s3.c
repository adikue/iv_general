    printf("count = %d\n", count);
    if (count <=  s1){
      //zone 1
      float n = (-1 + sqrt(1 + 8 * (double)count)) / 2;//Approximate number of diagonal
      n = n > trunc(n) ? trunc(n) + 1 : trunc(n);//Correcting number of diagonal
      int start_count = 1 + ((n-1)*(n-1) + (n-1))/2;//Start index for this diagonal
      int k = count - start_count;//Compute i of element
      int l = n - (count - start_count);//Compute j of element
      printf("[%d][%d]\n", k, l);
      a[k][l] = sin(0.00001*a[k+1][l-1]); 

    }else if (count > s1 && count <= s2){
      //zone 2
      double n = (double)(count - s1)/small_size;//Approximate number of diagonal
      n = n > trunc(n) ? trunc(n) + 1 : trunc(n);//Correcting number of diagonal
      int start_count = s1 + 1 + (n-1)*small_size;//Start index for this diagonal
      int k = n-1 + count - start_count;
      int l = JSIZE - (count - start_count) - 1;
      printf("[%d][%d]\n", k, l);
      a[k][l] = sin(0.00001*a[k+1][l-1]);

    }else{
      //zone 3
      int diff = count - s2;
      double tmp = 2*((double)small_size - 1) + 1;
      double n = ( tmp - sqrt(tmp*tmp - 8*(double)diff) ) / 2;//Approximate number of diagonal
      n = n > trunc(n) ? trunc(n) + 1 : trunc(n);//Correcting number of diagonal
      int start_count = 1 + s2 + (2*(small_size - 1) - n + 2)*(n - 1) / 2;
      int k = (b_diags + n) + (count - start_count) - 1;
      int l = JSIZE - (count - start_count) - 1;
      printf("[%d][%d]\n", k, l);
      a[k][l] = sin(0.00001*a[k+1][l-1]);    
    } 