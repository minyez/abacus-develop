#ifndef PSI_INITIALIZER_NAO_H
#define PSI_INITIALIZER_NAO_H
#include "psi_initializer.h"
#include "module_base/realarray.h"
#include "module_base/cubic_spline.h"
#include <memory>
/*
Psi (planewave based wavefunction) initializer: numerical atomic orbital method
*/
template <typename T, typename Device>
class psi_initializer_nao : public psi_initializer<T, Device>
{
    private:
        using Real = typename GetTypeReal<T>::type;
    public:
        psi_initializer_nao() {this->set_method("nao");};
        ~psi_initializer_nao() {};

        virtual void proj_ao_onkG(const int ik) override;

        #ifdef __MPI // MPI additional implementation
        /// @brief initialize the psi_initializer with external data and methods
        virtual void initialize(Structure_Factor*,                      //< structure factor
                                ModulePW::PW_Basis_K*,                  //< planewave basis
                                UnitCell*,                              //< unit cell
                                Parallel_Kpoints*,                      //< parallel kpoints
                                const int& = 1,                         //< random seed
                                pseudopot_cell_vnl* = nullptr,          //< nonlocal pseudopotential
                                const int& = 0) override;               //< MPI rank
        #else
        /// @brief serial version of initialize function, link psi_initializer with external data and methods
        virtual void initialize(Structure_Factor*,                      //< structure factor
                                ModulePW::PW_Basis_K*,                  //< planewave basis
                                UnitCell*,                              //< unit cell
                                const int& = 1,                         //< random seed
                                pseudopot_cell_vnl* = nullptr) override;//< nonlocal pseudopotential
        #endif

        void read_external_orbs(std::string* orbital_files, const int& rank);
        virtual void allocate_table() override;
        virtual void tabulate() override;
        
        std::vector<std::string> external_orbs() const { return orbital_files_; }
        std::vector<std::vector<int>> nr() const { return nr_; }
        std::vector<int> nr(const int& itype) const { return nr_[itype]; }
        int nr(const int& itype, const int& ichi) const { return nr_[itype][ichi]; }
        std::vector<std::vector<std::vector<double>>> chi() const { return chi_; }
        std::vector<std::vector<double>> chi(const int& itype) const { return chi_[itype]; }
        std::vector<double> chi(const int& itype, const int& ichi) const { return chi_[itype][ichi]; }
        double chi(const int& itype, const int& ichi, const int& ir) const { return chi_[itype][ichi][ir]; }
        std::vector<std::vector<std::vector<double>>> rgrid() const { return rgrid_; }
        std::vector<std::vector<double>> rgrid(const int& itype) const { return rgrid_[itype]; }
        std::vector<double> rgrid(const int& itype, const int& ichi) const { return rgrid_[itype][ichi]; }
        double rgrid(const int& itype, const int& ichi, const int& ir) const { return rgrid_[itype][ichi][ir]; }

    private:
        std::vector<std::string> orbital_files_;
        /// @brief cubic spline for interpolation
        std::unique_ptr<ModuleBase::CubicSpline> cubspl_;
        /// @brief radial map, [itype][l][izeta] -> i
        ModuleBase::realArray projmap_;
        /// @brief number of realspace grids per type per chi, [itype][ichi]
        std::vector<std::vector<int>> nr_;
        /// @brief data of numerical atomic orbital per type per chi per position, [itype][ichi][ir]
        std::vector<std::vector<std::vector<double>>> chi_;
        /// @brief r of numerical atomic orbital per type per chi per position, [itype][ichi][ir]
        std::vector<std::vector<std::vector<double>>> rgrid_;
};
#endif