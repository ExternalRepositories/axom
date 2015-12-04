/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */


#include <cstdlib>
#include <ctime>

#include <iostream>

#include "benchmark/benchmark_api.h"
#include "slic/slic.hpp"
#include "slic/UnitTestLogger.hpp"

//------------------------------------------------------------------------------
namespace {
    const int STRIDE = 7;
    const int OFFSET = 12;


    typedef int IndexType;
    typedef IndexType* IndexArray;

    typedef double DataType;
    typedef DataType* DataArray;

    // Generate an array of of size sz of indices in the range of [0,sz)
    // NOTE: Caller must delete the array
    IndexArray generateRandomPermutationArray(int sz, bool shouldPermute = false)
    {
        IndexArray indices = new IndexType[sz];

        for(IndexType i=0; i< sz; ++i)
        {
            indices[i] = i;
        }

        if(shouldPermute)
        {
            for(IndexType idx=0; idx< sz; ++idx)
            {
                // find a random position in the array and swap value with current idx
                IndexType otherIdx = idx + rand() % (sz - idx);
                SLIC_ASSERT(otherIdx >= idx && otherIdx < sz);
                std::swap(indices[idx], indices[otherIdx]);
            }

        }

//        for(IndexType i=0; i< sz; ++i)
//        {
//            SLIC_ASSERT(indices[i] >= 0 && indices[i] < sz);
//        }


        return indices;
    }

    // Generate an array of size sz of random doubles in the range [0,1)
    // NOTE: Caller must delete the array
    DataArray generateRandomDataField(int sz)
    {
        const DataType rMaxDouble = static_cast<DataType>(RAND_MAX);

        DataArray data = new DataType[sz];
        for(IndexType i=0; i< sz; ++i)
        {
            data[i] = rand() / rMaxDouble;;
        }


        for(IndexType i=0; i< sz; ++i)
        {
            SLIC_ASSERT(data[i] >= 0.0 && data[i] <= 1.0);
        }

        return data;
    }

    class SetFixture : public ::benchmark::Fixture
    {
    public:
        void SetUp() {

            volatile int str_vol = STRIDE;  // pass through volatile variable so the
            str = str_vol;                  // number is not a compile time constant

            volatile int off_vol = OFFSET;  // pass through volatile variable so the
            off = off_vol;                  // number is not a compile time constant

            ind = ATK_NULLPTR;
            data = ATK_NULLPTR;

        }

        void TearDown() {
            if(ind != ATK_NULLPTR)
                delete[] ind;
            if(data != ATK_NULLPTR)
                delete[] data;
        }

        ~SetFixture() {
            SLIC_ASSERT( ind == ATK_NULLPTR);
            SLIC_ASSERT( data == ATK_NULLPTR);
        }


        int maxIndex(int sz) { return (sz * str + off); }

        int off;
        int str;
        IndexArray ind;
        DataArray data;

    };

    enum ArrSizes  { S0 = 1<<3       // small
                   ,S1 = 1<<16      // larger than  32K L1 cache
                   ,S2 = 1<<19      // Larger than 256K L2 cache
                   ,S3 = 1<<25      // Larger than  25M L3 cache
                   };

    void CustomArgs(benchmark::internal::Benchmark* b) {
        b->Arg( S0) ;
        b->Arg( S1);
        b->Arg( S2);
        b->Arg( S3);
    }

}







/// -------------------


template<int SZ>
void contig_sequence_compileTimeSize(benchmark::State& state) {

    while (state.KeepRunning()) {
        for (int i=0; i < SZ; ++i) {
            int pos = i;
            benchmark::DoNotOptimize(pos);
      }
  }
  state.SetItemsProcessed(state.iterations() * SZ);
}
BENCHMARK_TEMPLATE(contig_sequence_compileTimeSize, S0);
BENCHMARK_TEMPLATE(contig_sequence_compileTimeSize, S1);
BENCHMARK_TEMPLATE(contig_sequence_compileTimeSize, S2);
BENCHMARK_TEMPLATE(contig_sequence_compileTimeSize, S3);


BENCHMARK_DEFINE_F(SetFixture, contig_sequence)(benchmark::State& state) {
    const int sz = state.range_x();
    int pos = 0;
    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          benchmark::DoNotOptimize(pos = i);
      }
  }
  state.SetItemsProcessed(state.iterations() * sz);
}
BENCHMARK_REGISTER_F(SetFixture, contig_sequence)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, strided_sequence)(benchmark::State& state) {
    const int sz = state.range_x();
    int pos = 0;

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          benchmark::DoNotOptimize(pos = i* str);
      }
  }
  state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, strided_sequence)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, offset_sequence)(benchmark::State& state) {
    const int sz = state.range_x();
    int pos =0;

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          benchmark::DoNotOptimize(pos = i + off);
      }
  }
  state.SetItemsProcessed(state.iterations() * sz);
}
BENCHMARK_REGISTER_F(SetFixture, offset_sequence)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, offset_strided_sequence) (benchmark::State& state) {
    const int sz = state.range_x();
    int pos = 0;

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          benchmark::DoNotOptimize(pos = i* str + off);
      }
    }
    state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, offset_strided_sequence)->Apply(CustomArgs);


BENCHMARK_DEFINE_F(SetFixture, indirection_sequence_ordered) (benchmark::State& state)
{
    const int sz = state.range_x();
    int pos = 0;
    ind = generateRandomPermutationArray( sz, false);

    while (state.KeepRunning())
    {
        for (int i=0; i < sz; ++i) {
          benchmark::DoNotOptimize( pos = ind[i] );
        }

    }

  state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, indirection_sequence_ordered)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, indirection_sequence_permuted) (benchmark::State& state)
{
    const int sz = state.range_x();
    int pos = 0;
    ind = generateRandomPermutationArray( sz, true);

    while (state.KeepRunning())
    {
        for (int i=0; i < sz; ++i) {
          benchmark::DoNotOptimize( pos = ind[i] );
        }
    }

  state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, indirection_sequence_permuted)->Apply(CustomArgs);


/// --------------------  Benchmarks for array indexing ---------------------
BENCHMARK_DEFINE_F(SetFixture, contig_sequence_field)(benchmark::State& state) {
    const int sz = state.range_x();
    data = generateRandomDataField( maxIndex(sz));

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          IndexType pos = i;
          benchmark::DoNotOptimize(data[pos]);
      }
  }
  state.SetItemsProcessed(state.iterations() * sz);
}
BENCHMARK_REGISTER_F(SetFixture, contig_sequence_field)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, strided_sequence_field)(benchmark::State& state) {
    const int sz = state.range_x();
    data = generateRandomDataField( maxIndex(sz));

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          IndexType pos = i*str;
          benchmark::DoNotOptimize( data[pos]);
      }
  }
  state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, strided_sequence_field)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, offset_sequence_field)(benchmark::State& state) {
    const int sz = state.range_x();
    data = generateRandomDataField( maxIndex(sz));

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
          IndexType pos = i + off;
          benchmark::DoNotOptimize(data[pos]);
      }
  }
  state.SetItemsProcessed(state.iterations() * sz);
}
BENCHMARK_REGISTER_F(SetFixture, offset_sequence_field)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, offset_strided_sequence_field) (benchmark::State& state) {
    const int sz = state.range_x();
    data = generateRandomDataField( maxIndex(sz));

    while (state.KeepRunning()) {
        for (int i=0; i < sz; ++i) {
            IndexType pos = i * str + off;
            benchmark::DoNotOptimize(data[pos]);
      }
    }
    state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, offset_strided_sequence_field)->Apply(CustomArgs);


BENCHMARK_DEFINE_F(SetFixture, indirection_sequence_ordered_field) (benchmark::State& state)
{
    const int sz = state.range_x();
    ind = generateRandomPermutationArray( sz, false);
    data = generateRandomDataField( maxIndex(sz));

//    if(sz == 8)
//    {
//        std::cout<<"\n array indices (order)\n\t";
//        for (int i=0; i < sz; ++i)
//            std::cout<< "<" << ind[i] << "," << data [ ind[i] ] <<">\t";
//        std::cout <<std::endl;
//    }



    while (state.KeepRunning())
    {
        for (int i=0; i < sz; ++i) {
          IndexType pos = ind[i];
          benchmark::DoNotOptimize( data[pos] );
        }

    }

  state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, indirection_sequence_ordered_field)->Apply(CustomArgs);

BENCHMARK_DEFINE_F(SetFixture, indirection_sequence_permuted_field) (benchmark::State& state)
{
    const int sz = state.range_x();
    ind = generateRandomPermutationArray( sz, true);
    data = generateRandomDataField( maxIndex(sz));

//    if(sz == 8)
//    {
//        std::cout<<"\n array indices (permute)\n\t";
//        for (int i=0; i < sz; ++i)
//            std::cout<< "<" << ind[i] << "," << data [ ind[i] ] <<">\t";
//        std::cout <<std::endl;
//    }




    while (state.KeepRunning())
    {
        for (int i=0; i < sz; ++i) {
          IndexType pos = ind[i];
          benchmark::DoNotOptimize( data[pos] );
        }
    }

  state.SetItemsProcessed(state.iterations() * sz);

}
BENCHMARK_REGISTER_F(SetFixture, indirection_sequence_permuted_field)->Apply(CustomArgs);

/// ----------------------------------------------------------------------------



int main(int argc, char * argv[])
{
  std::srand (std::time(NULL));
  asctoolkit::slic::UnitTestLogger logger;  // create & initialize test logger,

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  return 0;
}
