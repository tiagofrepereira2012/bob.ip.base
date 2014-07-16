/**
 * @date Sat Apr 30 17:52:15 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * @brief This file provides a class to smooth an image with a Gaussian kernel
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_IP_BASE_WEIGHTED_GAUSSIAN_H
#define BOB_IP_BASE_WEIGHTED_GAUSSIAN_H

#include "bob/core/assert.h"
#include "bob/core/cast.h"
#include "bob/sp/conv.h"
#include "bob/sp/extrapolate.h"

namespace bob { namespace ip { namespace base {

  /**
    * @brief This class allows to smooth images with a weighted Gaussian
    *        kernel (used by the Self Quotient Image)
    */
  class WeightedGaussian
  {
    public:
      /**
        * @brief Creates an object to smooth images with a weighted Gaussian
        *        kernel
        * @param radius_y The radius-height of the kernel along the y-axis
        *                 (height = 2*radius_y + 1)
        * @param radius_x The radius-width of the kernel along the x-axis
        *                 (width = 2*radius_x + 1)
        * @param sigma_y The variance of the kernel along the y-axis
        * @param sigma_x The variance of the kernel along the x-axis
        * @param border_type The interpolation type for the convolution
        */
      WeightedGaussian(
        const size_t radius_y=1, const size_t radius_x=1,
        const double sigma_y=sqrt(2.), const double sigma_x=sqrt(2.),
        const bob::sp::Extrapolation::BorderType border_type = bob::sp::Extrapolation::Mirror
      );

      /**
        * @brief Copy constructor
        */
      WeightedGaussian(const WeightedGaussian& other);

      /**
        * @brief Destructor
        */
      virtual ~WeightedGaussian() {}

      /**
        * @brief Assignment operator
        */
      WeightedGaussian& operator=(const WeightedGaussian& other);

      /**
        * @brief Equal to
        */
      bool operator==(const WeightedGaussian& b) const;
      /**
        * @brief Not equal to
        */
      bool operator!=(const WeightedGaussian& b) const;

      /**
        * @brief Resets the parameters of the filter
        * @param radius_y The radius-height of the kernel along the y-axis
        *                 (height = 2*radius_y + 1)
        * @param radius_x The radius-width of the kernel along the x-axis
        *                 (width = 2*radius_x + 1)
        * @param sigma_y The variance of the kernel along the y-axis
        * @param sigma_x The variance of the kernel along the x-axis
        * @param border_type The interpolation type for the convolution
        */
      void reset( const size_t radius_y=1, const size_t radius_x=1,
        const double sigma_y=sqrt(2.), const double sigma_x=sqrt(2.),
        const bob::sp::Extrapolation::BorderType border_type =
          bob::sp::Extrapolation::Mirror);

      /**
        * @brief Getters
        */
      size_t getRadiusY() const { return m_radius_y; }
      size_t getRadiusX() const { return m_radius_x; }
      double getSigmaY() const { return m_sigma_y; }
      double getSigmaX() const { return m_sigma_x; }
      bob::sp::Extrapolation::BorderType getConvBorder() const { return m_conv_border; }
      const blitz::Array<double,2>& getUnweightedKernel() const { return m_kernel; }

      /**
        * @brief Setters
        */
      void setRadiusY(const size_t radius_y){ m_radius_y = radius_y; computeKernel(); }
      void setRadiusX(const size_t radius_x){ m_radius_x = radius_x; computeKernel(); }
      void setRadius(const blitz::TinyVector<int,2>& radius) {m_radius_y = radius[0]; m_radius_x = radius[1]; computeKernel();}
      void setSigmaY(const double sigma_y){ m_sigma_y = sigma_y; computeKernel(); }
      void setSigmaX(const double sigma_x){ m_sigma_x = sigma_x; computeKernel(); }
      void setSigma(const blitz::TinyVector<double,2>& sigma) {m_sigma_y = sigma[0]; m_sigma_x = sigma[1]; computeKernel();}
      void setConvBorder(const bob::sp::Extrapolation::BorderType border_type){ m_conv_border = border_type; }


      /**
        * @brief Process a 2D blitz Array/Image
        * @param src The 2D input blitz array
        * @param dst The 2D output blitz array
        */
      void filter_(const blitz::Array<double,2>& src, blitz::Array<double,2>& dst);

      /**
        * @brief Process a 2D blitz Array/Image
        * @param src The 2D input blitz array
        * @param dst The 2D output blitz array
        */
      template <typename T>
      void filter(const blitz::Array<T,2>& src, blitz::Array<double,2>& dst){
        // Casts the input to double
        blitz::Array<double,2> src_d = bob::core::array::cast<double>(src);
        // Calls the specialized template function for double
        filter_(src_d, dst);
      }

      /**
        * @brief Process a 3D blitz Array/Image
        * @param src The 3D input blitz array
        * @param dst The 3D output blitz array
        */
      template <typename T>
      void filter(const blitz::Array<T,3>& src, blitz::Array<double,3>& dst){
        // Check number of planes
        bob::core::array::assertSameDimensionLength(src.extent(0), dst.extent(0));
        for( int p=0; p<dst.extent(0); ++p){
          const blitz::Array<T,2> src_slice = src( p, blitz::Range::all(), blitz::Range::all() );
          blitz::Array<double,2> dst_slice = dst( p, blitz::Range::all(), blitz::Range::all() );
          // Weighted Gaussian smooth plane
          filter(src_slice, dst_slice);
        }
      }

    private:
      void computeKernel();

      /**
        * @brief Attributes
        */
      size_t m_radius_y;
      size_t m_radius_x;
      double m_sigma_y;
      double m_sigma_x;
      bob::sp::Extrapolation::BorderType m_conv_border;

      blitz::Array<double,2> m_kernel;
      blitz::Array<double,2> m_kernel_weighted;

      blitz::Array<double,2> m_src_extra;
      blitz::Array<double,2> m_src_integral;
  };

} } } // namespaces

#endif // BOB_IP_BASE_WEIGHTED_GAUSSIAN_H

