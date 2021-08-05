#ifndef EXX_ABFS_PARALLEL_COMMUNICATE_DM3_H
#define EXX_ABFS_PARALLEL_COMMUNICATE_DM3_H

#include "exx_abfs-parallel.h"
#include "abfs-vector3_order.h"

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <atomic>

using namespace std;

class Exx_Abfs::Parallel::Communicate::DM3
{
public:
	void cal_DM(const double threshold_D);

	std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> DMr;

private:
	template<typename Tmatrix> std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>>
		K_to_R(const std::vector<Tmatrix> &DK_2D, const double threshold_D) const;
	const matrix &D_phase( const matrix &DK, const int ik, const Abfs::Vector3_Order<int> &box2) const;	
	matrix D_phase( const ComplexMatrix &DK, const int ik, const Abfs::Vector3_Order<int> &box2) const;	
//	std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>>
//		allreduce(const std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> &DR_a2D);

	class Allreduce
	{
		public:
			void init(
				const MPI_Comm &mpi_comm_in,
				const std::map<std::set<size_t>,std::set<size_t>> &H_atom_pairs_group);
			std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> a2D_to_exx(
				std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> &data_local) const;

		private:
			std::vector<std::map<size_t,std::set<size_t>>> H_atom_pairs_group_rank;	//H_atom_pairs_group_rank[rank][iat1][iat2]
			std::vector<size_t> send_size_list;
			size_t recv_size;

			MPI_Comm mpi_comm;
			int comm_sz;
			int my_rank;

			enum class Flag_Send {undo, begin_oar, finish_oar, begin_isend, finish_isend};
			enum class Flag_Recv {undo, begin_irecv, begin_iar, finish_iar};

			//std::vector<std::pair<bool,bool>> get_atom_in_2D() const;
			std::vector<std::map<size_t,std::set<size_t>>> get_H_atom_pairs_group_rank(
				const std::map<std::set<size_t>,std::set<size_t>> &H_atom_pairs_group) const;
			void get_send_recv_size(
				const std::vector<std::map<size_t,std::set<size_t>>> &H_atom_pairs_group_rank,
				const std::map<std::set<size_t>,std::set<size_t>> &H_atom_pairs_group,
				std::vector<size_t> &send_size_list, size_t &recv_size) const;
			void init_flags(
				std::vector<std::atomic<Flag_Send>> &flags_send,
				std::vector<std::atomic<Flag_Recv>> &flags_recv) const;
			bool finish_judge(
				const std::vector<std::atomic<Flag_Send>> &flags_send,
				const std::vector<std::atomic<Flag_Recv>> &flags_recv) const;
			bool memory_enough(
				const int rank_send_next,
				const std::vector<std::atomic<Flag_Send>> &flags_send) const;
			void send_data_process(
				const int rank_send_now,
				const std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> &data_local,
				std::vector<std::vector<double>> &oarps_isend,
				std::vector<std::atomic<Flag_Send>> &flags_send) const;
			std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix*>>>> get_intersection(
				const int rank_send_now,
				std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> &data_local) const;				
			void recv_data_process(
				const int rank_recv,
				std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> &data_all,
				std::vector<std::vector<double>> &iarps_irecv,
				std::vector<std::atomic<Flag_Recv>> &flags_recv,
				std::atomic_flag &lock_insert) const;
			template<typename M> void insert_data(
				std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,M>>>> &data_rank,
				std::vector<std::map<size_t,std::map<size_t,std::map<Abfs::Vector3_Order<int>,matrix>>>> &data_all) const;
			matrix& get_matrix(matrix&m) const;
			matrix& get_matrix(matrix*pm) const;
	};

public:
	Allreduce allreduce;
};

#endif